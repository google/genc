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

#ifndef GENC_CC_INTEROP_NETWORKING_HTTP_CLIENT_INTERFACE_H_
#define GENC_CC_INTEROP_NETWORKING_HTTP_CLIENT_INTERFACE_H_

#include <string>
#include "absl/status/statusor.h"

namespace genc {
namespace interop {
namespace networking {

class HttpClientInterface {
 public:
  virtual absl::StatusOr<std::string> GetFromUrl(
      const std::string& url) = 0;

  virtual absl::StatusOr<std::string> PostJsonToUrl(
      const std::string& url,
      const std::string& json_request) = 0;

  virtual absl::StatusOr<std::string> GetFromUrlAndSocket(
      const std::string& url,
      const std::string& socket_path) = 0;

  virtual absl::StatusOr<std::string> PostJsonToUrlAndSocket(
      const std::string& url,
      const std::string& socket_path,
      const std::string& json_request) = 0;

  virtual ~HttpClientInterface() {}
};

}  // namespace networking
}  // namespace interop
}  // namespace genc

#endif  // GENC_CC_INTEROP_NETWORKING_HTTP_CLIENT_INTERFACE_H_
