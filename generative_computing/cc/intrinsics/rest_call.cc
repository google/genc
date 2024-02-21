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

#include <string>

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "generative_computing/cc/modules/tools/curl_client.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace intrinsics {
absl::Status RestCall::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (intrinsic_pb.static_parameter().struct_().element_size() < 2) {
    return absl::InvalidArgumentError(
        "Expect RestCall contians URL (required) and API key (optional), "
        "method (default to POST when not set), missing parameters.");
  }
  return absl::OkStatus();
}

absl::Status RestCall::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                   const v0::Value& arg,
                                   v0::Value* result) const {
  const v0::Struct& args = intrinsic_pb.static_parameter().struct_();
  const std::string& method = args.element(0).str();
  const std::string& url = args.element(1).str();

  if (method == kRestCallGet) {
    *result = GENC_TRY(CurlClient::Get(url));
    return absl::OkStatus();
  }

  if (method == kRestCallPost) {
    const std::string& api_key =
        intrinsic_pb.static_parameter().struct_().element(2).str();
    const std::string& json_request_str = arg.str();

    *result = GENC_TRY(CurlClient::Post(api_key, url, json_request_str));
    return absl::OkStatus();
  }

  return absl::InvalidArgumentError(
      absl::StrFormat("Unsupported RestCall method: %s", method));
}

}  // namespace intrinsics
}  // namespace generative_computing
