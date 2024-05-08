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

#include "genc/cc/intrinsics/logical_not.h"

#include "absl/status/status.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace intrinsics {

absl::Status LogicalNot::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  return absl::OkStatus();
}

absl::Status LogicalNot::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                     const v0::Value& arg, v0::Value* result,
                                     Context* context) const {
  if (!arg.has_boolean()) {
    return absl::InvalidArgumentError("Argument does not contain boolean.");
  }

  result->set_boolean(!arg.boolean());
  return absl::OkStatus();
}

}  // namespace intrinsics
}  // namespace genc
