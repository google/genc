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

#include "genc/cc/modules/tools/wolfram_alpha.h"

#include <cstddef>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <curl/curl.h>
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

namespace {
// Callback fn to write the response.
size_t WriteCallback(void* contents, size_t size, size_t nmemb,
                     std::string* output) {
  size_t totalSize = size * nmemb;
  output->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}

// TODO(b/315872721): migrate all curl stuff to HttpClients.
// Calls Wolfram Alpha API to get a string response.
absl::StatusOr<std::string> CallShortAnswersAPI(const std::string& app_id,
                                                const std::string& query) {
  CURL* curl;
  CURLcode curl_code;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl == nullptr) return absl::InternalError("Unable to init CURL");

  char* escaped_query = curl_easy_escape(curl, query.c_str(), 0);

  std::string url =
      "http://api.wolframalpha.com/v2/query?appid=" + app_id +
      "&output=json&includepodid=Result&input=" + std::string(escaped_query);

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

  curl_code = curl_easy_perform(curl);

  // Error out if call fails
  if (curl_code != CURLE_OK) {
    return absl::InternalError(curl_easy_strerror(curl_code));
  }
  curl_easy_cleanup(curl);
  return readBuffer;
}

}  // namespace

namespace intrinsics {

absl::Status WolframAlpha::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_str()) {
    return absl::InvalidArgumentError("Expect template as appid, got none.");
  }
  return absl::OkStatus();
}

absl::Status WolframAlpha::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                       const v0::Value& arg,
                                       v0::Value* result) const {
  const std::string& appid = intrinsic_pb.static_parameter().str();
  std::string result_str = GENC_TRY(CallShortAnswersAPI(appid, arg.str()));
  result->set_str(result_str);
  return absl::OkStatus();
}

}  // namespace intrinsics
}  // namespace genc
