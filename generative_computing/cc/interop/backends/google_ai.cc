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

#include "generative_computing/cc/interop/backends/google_ai.h"

#include <cstddef>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/substitute.h"
#include <curl/curl.h>
#include "generative_computing/cc/intrinsics/model_inference_with_config.h"
#include "generative_computing/cc/modules/parsers/gemini_parser.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {

constexpr char kGeminiOnAIStudio[] = "/cloud/gemini";

// Callback fn to write the response.
size_t WriteCallback(void* contents, size_t size, size_t nmemb,
                     std::string* output) {
  size_t totalSize = size * nmemb;
  output->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}

absl::StatusOr<v0::Value> Post(const std::string& api_key,
                               const std::string& endpoint,
                               const std::string& json_request) {
  CURL* curl = curl_easy_init();

  if (curl == nullptr) return absl::InternalError("Unable to init CURL");

  curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());

  struct curl_slist* headers = nullptr;
  // API key can be embedded into the URL. Hence empty.
  if (!api_key.empty()) {
    headers = curl_slist_append(headers,
                                ("Authorization: Bearer " + api_key).c_str());
  }
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_request.c_str());

  // Set the callback function to put the curl response into a buffer.
  v0::Value response;
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, response.mutable_str());

  // Send the request
  CURLcode curl_code = curl_easy_perform(curl);

  // Error out if call fails
  if (curl_code != CURLE_OK) {
    return absl::InternalError(curl_easy_strerror(curl_code));
  }

  // Cleanup, free resource and return.
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return response;
}

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
    v0::Value response_json = GENC_TRY(Post(api_key, endpoint, input_json));

    // Extract text out of JSON
    return GeminiParser::GetTopCandidateAsText(response_json);
  };
  return absl::OkStatus();
}

}  // namespace generative_computing
