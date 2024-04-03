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

#include "genc/cc/interop/backends/java/wolfram_alpha_handler.h"

#include <string>

#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "genc/cc/interop/backends/java/wolfram_alpha_client_jni.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace intrinsics {

// Sends a HTTP request to Wolfram Alpha backend, using the HTTP settings
// provided in the 'func' value, and string request provided in the 'arg'
// value. Returns the response from the server as is.
absl::StatusOr<v0::Value> WolframAlphaHandler::call(
    const v0::Value& func, const v0::Value& arg) const {
  std::string config;
  func.SerializeToString(&config);
  const std::string& request = arg.str();

  std::string response_str = genc::CallWolframAlphaClient(
      jvm_, wolfram_alpha_client_, config, request);
  if (response_str.empty()) {
    LOG(ERROR) << "Error encountered in fetching response from Wolfram Alpha.";
    return absl::Status(absl::StatusCode::kInternal,
                        "Internal error in calling Wolfram Alpha client.");
  }

  v0::Value response_pb;
  response_pb.set_str(response_str);
  return response_pb;
}

absl::Status WolframAlphaHandler::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_str()) {
    return absl::InvalidArgumentError("Parameters missing required app id.");
  }
  return absl::OkStatus();
}

absl::Status WolframAlphaHandler::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                              const v0::Value& arg,
                                              v0::Value* result) const {
  v0::Value value_pb;
  (*value_pb.mutable_intrinsic()) = intrinsic_pb;
  v0::Value response = GENC_TRY(call(value_pb, arg));
  result->set_str(response.str());
  return absl::OkStatus();
}

}  // namespace intrinsics
}  // namespace genc
