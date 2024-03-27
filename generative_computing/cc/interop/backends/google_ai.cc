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

#include "generative_computing/cc/interop/backends/google_ai.h"

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/substitute.h"
#include "generative_computing/cc/intrinsics/model_inference_with_config.h"
#include "generative_computing/cc/modules/parsers/gemini_parser.h"
#include "generative_computing/cc/modules/tools/curl_client.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {

constexpr char kGeminiOnAIStudio[] = "/cloud/gemini";
}  // namespace

absl::Status GoogleAI::SetInferenceMap(
    intrinsics::ModelInferenceWithConfig::InferenceMap& inference_map) {
  inference_map[kGeminiOnAIStudio] =
      [](v0::Intrinsic intrinsic, v0::Value arg) -> absl::StatusOr<v0::Value> {
    std::string input_json = absl::Substitute(
        R"pb(
          {
            "contents":
            [ {
              "parts":
              [ { "text": "$0" }]
            }]
          }
        )pb",
        arg.str());
    const v0::Value& config = intrinsic.static_parameter().struct_().element(1);
    const std::string& endpoint = config.struct_().element(0).str() +
                                  "?key=" + config.struct_().element(1).str();
    std::string api_key = "";
    v0::Value response_json =
        GENC_TRY(CurlClient::Post(api_key, endpoint, input_json));

    // Extract text out of JSON
    return GeminiParser::GetTopCandidateAsText(response_json);
  };
  return absl::OkStatus();
}

}  // namespace generative_computing
