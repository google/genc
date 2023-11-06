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

#include "generative_computing/cc/intrinsics/breakable_chain.h"

#include <optional>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

absl::Status BreakableChain::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if ((intrinsic_pb.static_parameter().struct_().element_size() < 1)) {
    return absl::InvalidArgumentError(
        "Expect at least 1 function in chain, but got none.");
  }
  return absl::OkStatus();
}

absl::StatusOr<ControlFlowIntrinsicHandlerInterface::ValueRef>
BreakableChain::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                            std::optional<ValueRef> arg,
                            Context* context) const {
  auto params = intrinsic_pb.static_parameter().struct_().element();

  ValueRef state = arg.value();
  for (const auto& fn : params) {
    ValueRef fn_ref = GENC_TRY(context->CreateValue(fn.value()));
    ValueRef next_state = GENC_TRY(context->CreateCall(fn_ref, state));
    v0::Value next_state_pb;
    GENC_TRY(context->Materialize(next_state, &next_state_pb));
    if (!next_state_pb.has_boolean()) {
      state = next_state;
    } else if (next_state_pb.boolean()) {
      // If true, the chain will skip all following function invocations and
      // return state. If false, the state would be used as input to the next
      // function in chain.
      break;
    }
  }

  return state;
}

}  // namespace intrinsics
}  // namespace generative_computing
