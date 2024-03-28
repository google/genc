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
#include "genc/cc/interop/backends/android/mediapipe_llm_inference.h"

#include <jni.h>

#include <string>

#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "genc/cc/intrinsics/intrinsic_uris.h"
#include "genc/java/src/java/org/genc/interop/backends/mediapipe/jni/llm_inference_client_jni.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

namespace {
constexpr absl::string_view kTestModelUri = "/cloud/testing";
}  // namespace

absl::StatusOr<v0::Value> MediapipeLlmInferenceCall(
    JavaVM* jvm, jobject llm_inference_client, const v0::Value& func,
    const v0::Value& arg) {
  if (func.has_intrinsic() &&
      (func.intrinsic().uri() == intrinsics::kModelInference) &&
      (func.intrinsic().static_parameter().str() == kTestModelUri)) {
    v0::Value resp_pb;
    resp_pb.set_str(
        absl::StrCat("Testing on device model with prompt: ", arg.str()));
    return resp_pb;
  }

  std::string model_inference_with_config;
  func.SerializeToString(&model_inference_with_config);
  std::string text_input = arg.str();
  LOG(INFO) << "Model inference with config: " << func.DebugString()
            << "Text input to Mediapipe LLM Inference: " << text_input;

  auto ts = absl::Now();
  std::string response = genc::CallLlmInferenceClient(
      jvm, llm_inference_client, model_inference_with_config, text_input);
  LOG(INFO) << "Response time:" << absl::Now() - ts;
  if (response.empty()) {
    LOG(ERROR) << "Error encountered in getting result from Mediapipe client.";
    return absl::Status(absl::StatusCode::kInternal,
                        "Internal error in calling Mediapipe client");
  }

  v0::Value response_pb;
  response_pb.set_str(response);
  return response_pb;
}

}  // namespace genc
