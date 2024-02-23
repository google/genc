/* Copyright 2023, The Generative Computing Authors.

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

#include "generative_computing/cc/examples/executors/android/android_executor_stacks.h"

#include <jni.h>

#include <memory>
#include <string>
#include <utility>

#include "absl/base/call_once.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/authoring/constructor.h"
#include "generative_computing/cc/interop/backends/android/google_ai.h"
#include "generative_computing/cc/interop/backends/android/mediapipe_llm_inference.h"
#include "generative_computing/cc/interop/backends/android/open_ai.h"
#include "generative_computing/cc/interop/backends/android/wolfram_alpha_handler.h"
#include "generative_computing/cc/intrinsics/handler_sets.h"
#include "generative_computing/cc/modules/agents/react.h"
#include "generative_computing/cc/modules/retrieval/local_cache.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/executor_stacks.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

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

static absl::once_flag context_init_flag;
static ExecutorStacksContext* executor_stacks_context = nullptr;
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
  config->model_inference_map[std::string(model_uri)] =
      [jvm, open_ai_client,
       model_uri](v0::Value arg) -> absl::StatusOr<v0::Value> {
    return generative_computing::OpenAiCall(
        jvm, open_ai_client, GENC_TRY(CreateModelInference(model_uri)), arg);
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
    return generative_computing::GoogleAiCall(jvm, google_ai_client,
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
    return generative_computing::MediapipeLlmInferenceCall(
        jvm, llm_inference_client, model_inference, arg);
  };
}

void SetWolframAlphaIntrinsicHandler(intrinsics::HandlerSetConfig* config,
                                     JavaVM* jvm,
                                     jobject wolfram_alpha_client) {
  config->custom_intrinsics_list.push_back(
      new generative_computing::intrinsics::WolframAlphaHandler(
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
  SetWolframAlphaIntrinsicHandler(&config, jvm, wolfram_alpha_client);

  // ReAct functions.
  GENC_TRY(ReAct::SetCustomFunctions(config.custom_function_map));

  // Memory functions.
  GENC_TRY(SetCustomFunctionsForLocalValueCache(
      config.custom_function_map, *executor_stacks_context->local_cache_));

  return CreateLocalExecutor(intrinsics::CreateCompleteHandlerSet(config));
}

}  // namespace generative_computing
