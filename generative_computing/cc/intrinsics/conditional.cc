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

#include "generative_computing/cc/intrinsics/conditional.h"

#include <optional>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

absl::Status Conditional::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (intrinsic_pb.static_parameter().struct_().element_size() != 2) {
    return absl::InvalidArgumentError("Missing a pair of static parameters.");
  }
  if ((intrinsic_pb.static_parameter().struct_().element(0).name() != "then") ||
      (intrinsic_pb.static_parameter().struct_().element(1).name() != "else")) {
    return absl::InvalidArgumentError("Wrong static parameter names.");
  }
  return absl::OkStatus();
}

absl::StatusOr<ControlFlowIntrinsicHandlerInterface::ValueRef>
Conditional::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                         std::optional<ValueRef> arg, Context* context) const {
  if (!arg.has_value()) {
    return absl::InvalidArgumentError("Missing condition.");
  }
  v0::Value cond_pb;
  GENC_TRY(context->Materialize(arg.value(), &cond_pb));
  if (!cond_pb.has_boolean()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Condition is not a Boolean: ", cond_pb.DebugString()));
  }
  return context->CreateValue(
      cond_pb.boolean()
          ? intrinsic_pb.static_parameter().struct_().element(0).value()
          : intrinsic_pb.static_parameter().struct_().element(1).value());
}

}  // namespace intrinsics
}  // namespace generative_computing
