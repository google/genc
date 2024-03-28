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

#include "genc/cc/intrinsics/repeated_conditional_chain.h"

#include <optional>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "genc/cc/runtime/intrinsic_handler.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace intrinsics {

absl::Status RepeatedConditionalChain::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_struct_() ||
      intrinsic_pb.static_parameter().struct_().element_size() < 2) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Expect at least 2 elementes in static parameter 'num_steps' "
        "and a listed of body_fns, got: ",
        intrinsic_pb.static_parameter().DebugString()));
  }
  return absl::OkStatus();
}

absl::StatusOr<ControlFlowIntrinsicHandlerInterface::ValueRef>
RepeatedConditionalChain::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                      std::optional<ValueRef> arg,
                                      Context* context) const {
  int num_steps = intrinsic_pb.static_parameter().struct_().element(0).int_32();

  auto params = intrinsic_pb.static_parameter().struct_().element();
  std::vector<ValueRef> body_fns_ref;

  // First param is reserved for num_steps
  for (auto it = params.begin() + 1; it != params.end(); it++) {
    body_fns_ref.push_back(GENC_TRY(context->CreateValue(*it)));
  }

  ValueRef state = arg.value();
  bool should_break = false;
  for (int i = 0; i < num_steps; i++) {
    // TODO(b/309026999): generalize this as a breakable chain intrinsics.
    for (const auto& body_fn : body_fns_ref) {
      ValueRef fn_val = GENC_TRY(context->CreateCall(body_fn, state));
      v0::Value next_state_pb;
      GENC_TRY(context->Materialize(fn_val, &next_state_pb));
      // TODO(b/309026999): make stop condition an intrinsic
      // TODO(b/309026999): handle variants.
      if (!next_state_pb.has_boolean()) {
        state = GENC_TRY(context->CreateValue(next_state_pb));
      } else if (next_state_pb.boolean()) {
        should_break = true;
        break;
      }
    }
    if (should_break) {
      break;
    }
  }
  return state;
}

}  // namespace intrinsics
}  // namespace genc
