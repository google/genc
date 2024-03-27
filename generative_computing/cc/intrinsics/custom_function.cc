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

#include "generative_computing/cc/intrinsics/custom_function.h"

#include <string>

#include "absl/status/status.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

absl::Status CustomFunction::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_str()) {
    return absl::InvalidArgumentError("Expect fn_uri as str, got none.");
  }
  return absl::OkStatus();
}

absl::Status CustomFunction::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                         const v0::Value& arg,
                                         v0::Value* result) const {
  const std::string& fn_uri = intrinsic_pb.static_parameter().str();
  auto fn_it = function_map_.find(fn_uri);
  if (fn_it != function_map_.end()) {
    *result = GENC_TRY(fn_it->second(arg));
  }
  return absl::OkStatus();
}

}  // namespace intrinsics
}  // namespace generative_computing
