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

#include "generative_computing/cc/intrinsics/delegate.h"

#include <optional>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

absl::Status Delegate::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_struct_()) {
    return absl::InvalidArgumentError("Expect a struct.");
  }
  if (!intrinsic_pb.static_parameter().struct_().element(0).value().has_str()) {
    return absl::InvalidArgumentError("Expected an environment name string.");
  }
  return absl::OkStatus();
}

absl::StatusOr<ControlFlowIntrinsicHandlerInterface::ValueRef>
Delegate::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                      std::optional<ValueRef> arg, Context* context) const {
  const std::string& environment_name =
      intrinsic_pb.static_parameter().struct_().element(0).value().str();
  if (!runner_map_.contains(environment_name)) {
    return absl::NotFoundError(
        absl::StrCat("No such runtime environment: ", environment_name));
  }
  RunnerFn runner_fn = runner_map_.at(environment_name);
  const v0::Value& func_pb =
      intrinsic_pb.static_parameter().struct_().element(1).value();
  std::optional<v0::Value> optional_arg_pb;
  if (arg.has_value()) {
    v0::Value arg_pb;
    GENC_TRY(context->Materialize(arg.value(), &arg_pb));
    optional_arg_pb->Swap(&arg_pb);
  } else {
    optional_arg_pb = std::nullopt;
  }
  v0::Value result_pb;
  GENC_TRY(runner_fn(func_pb, optional_arg_pb, &result_pb));
  return context->CreateValue(result_pb);
}

}  // namespace intrinsics
}  // namespace generative_computing
