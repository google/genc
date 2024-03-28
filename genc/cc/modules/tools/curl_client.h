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
#ifndef GENC_CC_MODULES_TOOLS_CURL_CLIENT_H_
#define GENC_CC_MODULES_TOOLS_CURL_CLIENT_H_

#include <string>
#include "absl/status/statusor.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

// Makes REST calls via CURL.
class CurlClient final {
 public:
  ~CurlClient() = default;

  // POST request, supports OAUTH and non OAUTH where API key is embedded in
  // endpoint.
  static absl::StatusOr<v0::Value> Post(const std::string& api_key,
                                        const std::string& endpoint,
                                        const std::string& json_request);

  // GET request, API key is embedded in the URL.
  static absl::StatusOr<v0::Value> Get(const std::string& endpoint);

      // Not copyable or movable.
      CurlClient(const CurlClient&) = delete;
  CurlClient& operator=(const CurlClient&) = delete;

 private:
  // Do not hold states in this class.
  CurlClient() = default;
};
}  // namespace genc

#endif  // GENC_CC_MODULES_TOOLS_CURL_CLIENT_H_
