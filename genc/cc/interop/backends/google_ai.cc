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

#include "genc/cc/interop/backends/google_ai.h"

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/substitute.h"
#include "genc/cc/intrinsics/model_inference_with_config.h"
#include "genc/cc/modules/parsers/gemini_parser.h"
#include "genc/cc/modules/tools/curl_client.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"
#include <nlohmann/json.hpp>

namespace genc {

namespace {

constexpr char kGeminiOnAIStudio[] = "/cloud/gemini";
}  // namespace

absl::StatusOr<std::string> updateJsonRequest(
    const std::string& json_request_template, const std::string& request_str) {
  nlohmann::json request_template = nlohmann::json::parse(
      json_request_template, /*cb=*/nullptr, /*allow_exceptions=*/false);
  if (!request_template.is_object()) {
    return absl::Status(
        absl::StatusCode::kInvalidArgument,
        "Failed to parse json request template: " + json_request_template);
  }

  // Check if 'contents' key is present
  if (!request_template.contains("contents") ||
      request_template["contents"].empty()) {
    return absl::Status(
        absl::StatusCode::kInvalidArgument,
        "Invalid json request template, 'contents' not found: " +
            json_request_template);
  }

  // Get the last 'contents' object
  auto& lastContent = request_template["contents"].back();

  // Check if 'parts' key is present
  if (!lastContent.contains("parts") || lastContent["parts"].empty()) {
    return absl::Status(absl::StatusCode::kInvalidArgument,
                        "Invalid json request template, 'parts' not found: " +
                            json_request_template);
  }

  // Get the last element in 'parts' array and update the 'text' value.
  auto& lastPart = lastContent["parts"].back();
  lastPart["text"] = request_str;
  return request_template.dump();
}

absl::Status GoogleAI::SetInferenceMap(
    intrinsics::ModelInferenceWithConfig::InferenceMap& inference_map) {
  inference_map[kGeminiOnAIStudio] =
      [](v0::Intrinsic intrinsic, v0::Value arg) -> absl::StatusOr<v0::Value> {
    const v0::Value& config = intrinsic.static_parameter().struct_().element(1);

    std::string endpoint;
    std::string api_key;
    std::string json_request_template = R"pb(
      {
        "contents":
        [ {
          "parts":
          [ { "text": "$0" }]
        }]
      }
    )pb";

    for (const v0::Value& param : config.struct_().element()) {
      if (param.label() == "endpoint") {
        endpoint = param.str();
      } else if (param.label() == "api_key") {
        api_key = param.str();
      } else if (param.label() == "json_request_template" &&
                 !param.str().empty()) {
        json_request_template = param.str();
      }
    }

    absl::StatusOr<std::string> input_json =
        updateJsonRequest(json_request_template, arg.str());
    if (!input_json.ok()) {
      return input_json.status();
    }

    const std::string& endpointUrl = endpoint + "?key=" + api_key;

    // Don't send api_key to Curl client as it has been incorporated in the
    // endpointUrl as query param
    api_key = "";
    v0::Value response_json =
        GENC_TRY(CurlClient::Post(api_key, endpointUrl, input_json.value()));

    // Extract text out of JSON
    return GeminiParser::GetTopCandidateAsText(response_json);
    return absl::OkStatus();
  };
  return absl::OkStatus();
}

}  // namespace genc
