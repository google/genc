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

#include "generative_computing/cc/runtime/android/android_executor_stacks.h"

#include <jni.h>

#include <memory>
#include <string>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/authoring/constructor.h"
#include "generative_computing/cc/interop/backends/android/google_ai.h"
#include "generative_computing/cc/interop/backends/android/mediapipe_llm_inference.h"
#include "generative_computing/cc/interop/backends/android/open_ai.h"
#include "generative_computing/cc/intrinsics/handler_sets.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/executor_stacks.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {
constexpr absl::string_view kOpenAIModelUri = "/openai/chatgpt";
constexpr absl::string_view kGeminiModelUri = "/cloud/gemini";
constexpr absl::string_view kMediapipeModelUri = "/device/llm_inference";
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

}  // namespace

absl::StatusOr<std::shared_ptr<Executor>> CreateAndroidExecutor(
    JavaVM* jvm, jobject open_ai_client, jobject google_ai_client,
    jobject llm_inference_client) {
  intrinsics::HandlerSetConfig config;
  SetOpenAiModelInferenceHandler(&config, jvm, open_ai_client, kOpenAIModelUri);
  SetGoogleAiModelInferenceHandler(&config, jvm, google_ai_client,
                                   kGeminiModelUri);
  SetMediapipeModelInferenceHandler(&config, jvm, llm_inference_client,
                                    kMediapipeModelUri);
  return CreateLocalExecutor(intrinsics::CreateCompleteHandlerSet(config));
}

}  // namespace generative_computing
