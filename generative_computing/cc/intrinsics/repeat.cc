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

#include "generative_computing/cc/intrinsics/repeat.h"

#include <optional>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

absl::Status Repeat::CheckWellFormed(const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_struct_() ||
      intrinsic_pb.static_parameter().struct_().element_size() != 2) {
    return absl::InvalidArgumentError(
        "Expext struct with 2 elements for Repeat.");
  }
  return absl::OkStatus();
}

absl::StatusOr<ControlFlowIntrinsicHandlerInterface::ValueRef>
Repeat::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                    std::optional<ValueRef> arg, Context* context) const {
  int num_steps = intrinsic_pb.static_parameter().struct_().element(0).int_32();

  ValueRef body_fn = GENC_TRY(context->CreateValue(
      intrinsic_pb.static_parameter().struct_().element(1)));

  ValueRef state = arg.value();

  for (int i = 0; i < num_steps; i++) {
    state = GENC_TRY(context->CreateCall(body_fn, state));
  }
  return state;
}
}  // namespace intrinsics
}  // namespace generative_computing
