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

#include "generative_computing/cc/intrinsics/fallback.h"

#include <optional>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

absl::Status Fallback::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  return absl::OkStatus();
}

absl::StatusOr<ControlFlowIntrinsicHandlerInterface::ValueRef>
Fallback::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                      std::optional<ValueRef> arg, Context* context) const {
  absl::Status exe_status =
      absl::UnavailableError("No candidate computations available.");
  for (const v0::Value& fn_pb :
       intrinsic_pb.static_parameter().struct_().element()) {
    ValueRef result = GENC_TRY(context->CreateValue(fn_pb));
    result = GENC_TRY(context->CreateCall(result, arg));
    v0::Value result_pb;
    exe_status = context->Materialize(result, &result_pb);
    if (exe_status.ok()) {
      return context->CreateValue(result_pb);
    }
  }
  return exe_status;
}

}  // namespace intrinsics
}  // namespace generative_computing
