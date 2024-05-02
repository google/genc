/* Copyright 2023, The GenC Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License
==============================================================================*/

#include "genc/cc/examples/executors/android/android_executor_stacks.h"

#include <jni.h>

#include <memory>
#include <string>
#include <utility>

#include "absl/base/call_once.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "genc/cc/interop/backends/java/google_ai.h"
#include "genc/cc/interop/backends/java/mediapipe_llm_inference.h"
#include "genc/cc/interop/backends/java/open_ai.h"
#include "genc/cc/interop/backends/java/wolfram_alpha_handler.h"
#include "genc/cc/interop/backends/llamacpp.h"
#include "genc/cc/intrinsics/handler_sets.h"
#include "genc/cc/modules/agents/react.h"
#include "genc/cc/modules/retrieval/local_cache.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/executor_stacks.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

// Stateful context that holds state (e.g. memory), which need to remain
// alive after initialization.
struct ExecutorStacksContext {
  ExecutorStacksContext(std::unique_ptr<LocalValueCache> local_cache)
      : local_cache_(std::move(local_cache)) {}
  std::unique_ptr<LocalValueCache> local_cache_;
};

namespace {
constexpr absl::string_view kOpenAIModelUri = "/openai/chatgpt";
constexpr absl::string_view kGeminiModelUri = "/cloud/gemini";
constexpr absl::string_view kMediapipeModelUri = "/device/llm_inference";
constexpr absl::string_view kLlamaCppModelUri = "/device/llamacpp";

static absl::once_flag context_init_flag;
static ExecutorStacksContext* executor_stacks_context = nullptr;
LlamaCpp* llama_cpp_client = nullptr;
constexpr int MAX_CACHE_SIZE_PER_KEY = 200;

static void InitExecutorStacksContext() {
  auto local_cache = std::make_unique<LocalValueCache>(MAX_CACHE_SIZE_PER_KEY);
  executor_stacks_context = new ExecutorStacksContext(std::move(local_cache));
}
}  // namespace

namespace {

void SetOpenAiModelInferenceHandler(intrinsics::HandlerSetConfig* config,
                                    JavaVM* jvm, jobject open_ai_client,
                                    absl::string_view model_uri) {
  config->model_inference_with_config_map[std::string(model_uri)] =
      [jvm, open_ai_client](v0::Intrinsic intrinsic,
                            v0::Value arg) -> absl::StatusOr<v0::Value> {
    v0::Value model_inference;
    (*model_inference.mutable_intrinsic()) = intrinsic;
    return genc::OpenAiCall(jvm, open_ai_client,
                                            model_inference, arg);
  };
}

void SetGoogleAiModelInferenceHandler(intrinsics::HandlerSetConfig* config,
                                      JavaVM* jvm, jobject google_ai_client,
                                      absl::string_view model_uri) {
  config->model_inference_with_config_map[std::string(model_uri)] =
      [jvm, google_ai_client](
          v0::Intrinsic intrinsic, v0::Value arg) -> absl::StatusOr<v0::Value> {
    v0::Value model_inference;
    (*model_inference.mutable_intrinsic()) = intrinsic;
    return genc::GoogleAiCall(jvm, google_ai_client,
                                              model_inference, arg);
  };
}

void SetMediapipeModelInferenceHandler(intrinsics::HandlerSetConfig* config,
                                       JavaVM* jvm,
                                       jobject llm_inference_client,
                                       absl::string_view model_uri) {
  config->model_inference_with_config_map[std::string(model_uri)] =
      [jvm, llm_inference_client](v0::Intrinsic intrinsic,
                                  v0::Value arg) -> absl::StatusOr<v0::Value> {
    v0::Value model_inference;
    (*model_inference.mutable_intrinsic()) = intrinsic;
    return genc::MediapipeLlmInferenceCall(
        jvm, llm_inference_client, model_inference, arg);
  };
}

void SetLlamaCppModelInferenceHandler(intrinsics::HandlerSetConfig* config,
                                      absl::string_view model_uri) {
  config->model_inference_with_config_map[std::string(model_uri)] =
      [](v0::Intrinsic intrinsic, v0::Value arg) -> absl::StatusOr<v0::Value> {
    v0::Value model_inference;
    if (!llama_cpp_client) {
      // If the model hasn't been initialized, parse the config from
      // the intrinsic and create the model.
      const v0::Value& config =
          intrinsic.static_parameter().struct_().element(1);

      llama_cpp_client = new LlamaCpp();
      absl::Status status = llama_cpp_client->InitModel(config);
      if (!status.ok()) {
        return status;
      }
    }
    return llama_cpp_client->LlamaCppCall(arg);
  };
}

void SetWolframAlphaIntrinsicHandler(intrinsics::HandlerSetConfig* config,
                                     JavaVM* jvm,
                                     jobject wolfram_alpha_client) {
  config->custom_intrinsics_list.push_back(
      new genc::intrinsics::WolframAlphaHandler(
          jvm, wolfram_alpha_client));
}
}  // namespace

absl::StatusOr<std::shared_ptr<Executor>> CreateAndroidExecutor(
    JavaVM* jvm, jobject open_ai_client, jobject google_ai_client,
    jobject llm_inference_client, jobject wolfram_alpha_client) {
  // Initialize only once.
  absl::call_once(context_init_flag, InitExecutorStacksContext);

  intrinsics::HandlerSetConfig config;
  SetOpenAiModelInferenceHandler(&config, jvm, open_ai_client, kOpenAIModelUri);
  SetGoogleAiModelInferenceHandler(&config, jvm, google_ai_client,
                                   kGeminiModelUri);
  SetMediapipeModelInferenceHandler(&config, jvm, llm_inference_client,
                                    kMediapipeModelUri);

  SetLlamaCppModelInferenceHandler(&config, kLlamaCppModelUri);

  SetWolframAlphaIntrinsicHandler(&config, jvm, wolfram_alpha_client);

  // ReAct functions.
  GENC_TRY(ReAct::SetCustomFunctions(config.custom_function_map));

  // Memory functions.
  GENC_TRY(SetCustomFunctionsForLocalValueCache(
      config.custom_function_map, *executor_stacks_context->local_cache_));

  return CreateLocalExecutor(intrinsics::CreateCompleteHandlerSet(config));
}

}  // namespace genc
