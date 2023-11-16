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
#include "absl/strings/str_cat.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
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
  absl::Status error_status =
      absl::UnavailableError("No candidate computations available.");
  for (const v0::Value& fn_pb :
       intrinsic_pb.static_parameter().struct_().element()) {
    absl::StatusOr<ValueRef> result =
        context->CreateValue(fn_pb);
    if (result.ok()) {
      result = context->CreateCall(result.value(), arg);
      if (result.ok()) {
        error_status = context->Materialize(result.value(), nullptr);
        if (error_status.ok()) {
          return result;
        }
      } else {
        error_status = result.status();
      }
    } else {
      error_status = result.status();
    }
  }
  return error_status;
}

}  // namespace intrinsics
}  // namespace generative_computing
