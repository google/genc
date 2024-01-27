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

#include "generative_computing/cc/intrinsics/rest_call.h"

#include <cstddef>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <curl/curl.h>
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {

namespace {

// Callback fn to write the response.
size_t WriteCallback(void* contents, size_t size, size_t nmemb,
                     std::string* output) {
  size_t totalSize = size * nmemb;
  output->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}
}  // namespace

absl::StatusOr<v0::Value> Call(const std::string& api_key,
                               const std::string& endpoint,
                               const std::string& json_request) {
  CURL* curl = curl_easy_init();

  if (curl == nullptr) return absl::InternalError("Unable to init CURL");

  curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());

  struct curl_slist* headers = nullptr;
  headers =
      curl_slist_append(headers, ("Authorization: Bearer " + api_key).c_str());
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_request.c_str());

  // Set the callback function to put the curl response into a buffer.
  v0::Value response;
  std::string* response_json = response.mutable_str();
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_json);

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

namespace intrinsics {
absl::Status RestCall::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (intrinsic_pb.static_parameter().struct_().element_size() != 2) {
    return absl::InvalidArgumentError(
        "Expect RestCall contians URL and API key, missing parameters.");
  }
  return absl::OkStatus();
}

absl::Status RestCall::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                   const v0::Value& arg,
                                   v0::Value* result) const {
  const std::string& url =
      intrinsic_pb.static_parameter().struct_().element(0).str();
  const std::string& api_key =
      intrinsic_pb.static_parameter().struct_().element(1).str();
  const std::string& json_request_str = arg.str();

  *result = GENC_TRY(Call(api_key, url, json_request_str));
  return absl::OkStatus();
}

}  // namespace intrinsics
}  // namespace generative_computing
