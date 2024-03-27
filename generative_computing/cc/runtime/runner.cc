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

#include "generative_computing/cc/runtime/runner.h"

#include <memory>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/executor_stacks.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

absl::StatusOr<Runner> Runner::CreateDefault() {
  return Runner::Create(/*executor=*/nullptr);
}

absl::StatusOr<Runner> Runner::Create(std::shared_ptr<Executor> executor) {
  if (executor == nullptr) {
    executor = GENC_TRY(CreateDefaultLocalExecutor());
  }
  return Runner(nullptr, executor);
}

absl::StatusOr<Runner> Runner::Create(v0::Value computation) {
  return Create(computation, /*executor=*/nullptr);
}

absl::StatusOr<Runner> Runner::Create(v0::Value computation,
                                     std::shared_ptr<Executor> executor) {
  if (executor == nullptr) {
    executor = GENC_TRY(CreateDefaultLocalExecutor());
  }
  return Runner(std::make_unique<v0::Value>(computation), executor);
}

absl::StatusOr<v0::Value> Runner::Run(v0::Value arg) {
  if (computation_or_null_ == nullptr) {
    return absl::InvalidArgumentError(
        "A computation was not provided in the constructor.");
  }
  return RunInternal(*computation_or_null_, arg);
}

absl::StatusOr<v0::Value> Runner::Run(v0::Value computation, v0::Value arg) {
  if (computation_or_null_ != nullptr) {
    return absl::InvalidArgumentError(
        "A computation was already provided in the constructor.");
  }
  return RunInternal(computation, arg);
}

absl::StatusOr<v0::Value> Runner::RunInternal(
    v0::Value computation, v0::Value arg) {
  OwnedValueId comp_val = GENC_TRY(executor_->CreateValue(computation));
  OwnedValueId arg_val = GENC_TRY(executor_->CreateValue(arg));
  OwnedValueId result_val =
      GENC_TRY(executor_->CreateCall(comp_val.ref(), arg_val.ref()));
  v0::Value result;
  absl::Status status = executor_->Materialize(result_val.ref(), &result);
  if (!status.ok()) {
    return status;
  }
  return result;
}

}  // namespace generative_computing
