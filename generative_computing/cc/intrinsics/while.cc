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

#include "generative_computing/cc/intrinsics/while.h"

#include <optional>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

absl::Status While::CheckWellFormed(const v0::Intrinsic& intrinsic_pb) const {
  if (intrinsic_pb.static_parameter().struct_().element_size() != 2) {
    return absl::InvalidArgumentError("Missing required static parameters.");
  }
  if ((intrinsic_pb.static_parameter().struct_().element(0).name() !=
       "condition_fn") ||
      (intrinsic_pb.static_parameter().struct_().element(1).name() !=
       "body_fn")) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Wrong static parameter names."
        "Expected element 0: 'condition_fn' and element 1: 'body_fn'."
        "Received: ",
        intrinsic_pb.static_parameter().DebugString()));
  }
  return absl::OkStatus();
}

absl::StatusOr<ControlFlowIntrinsicHandlerInterface::ValueRef>
While::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                   std::optional<ValueRef> arg, Context* context) const {
  ValueRef condition_fn = GENC_TRY(context->CreateValue(
      intrinsic_pb.static_parameter().struct_().element(0).value()));

  ValueRef body_fn = GENC_TRY(context->CreateValue(
      intrinsic_pb.static_parameter().struct_().element(1).value()));

  bool condition_result = true;
  ValueRef state = arg.value();

  while (condition_result) {
    ValueRef condition_val;
    condition_val = GENC_TRY(context->CreateCall(condition_fn, state));
    v0::Value cond_pb;
    GENC_TRY(context->Materialize(condition_val, &cond_pb));
    if (!cond_pb.has_boolean()) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Condition does not have boolean: ", cond_pb.DebugString()));
    }
    condition_result = cond_pb.boolean();
    if (!condition_result) {
      break;
    } else {
      state = GENC_TRY(context->CreateCall(body_fn, state));
    }
  }

  return state;
}

}  // namespace intrinsics
}  // namespace generative_computing
