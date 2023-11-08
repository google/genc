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

#include "generative_computing/cc/intrinsics/parallel_map.h"

#include <optional>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/types/span.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

absl::Status ParallelMap::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.has_static_parameter()) {
    return absl::InvalidArgumentError(
        "Expect exactly one static_parameter for ParallelMap, got None.");
  }

  return absl::OkStatus();
}

absl::StatusOr<ControlFlowIntrinsicHandlerInterface::ValueRef>
ParallelMap::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                         std::optional<ValueRef> arg, Context* context) const {
  v0::Value args_pb;
  // TODO(b/309696962): remove after type support.
  GENC_TRY(context->Materialize(arg.value(), &args_pb));
  std::vector<ValueRef> results;
  results.reserve(args_pb.struct_().element_size());
  ValueRef fn_val =
      GENC_TRY(context->CreateValue(intrinsic_pb.static_parameter()));
  for (const v0::NamedValue& e : args_pb.struct_().element()) {
    ValueRef arg_val = GENC_TRY(context->CreateValue(e.value()));
    results.push_back(GENC_TRY(context->CreateCall(fn_val, arg_val)));
  }
  return context->CreateStruct(absl::MakeSpan(results));
}

}  // namespace intrinsics
}  // namespace generative_computing
