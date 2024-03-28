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

#include "genc/cc/runtime/executor_stacks.h"

#include <memory>

#include "absl/status/statusor.h"
#include "genc/cc/intrinsics/handler_sets.h"
#include "genc/cc/runtime/control_flow_executor.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/inline_executor.h"
#include "genc/cc/runtime/intrinsic_handler.h"
#include "genc/cc/runtime/status_macros.h"

namespace genc {

absl::StatusOr<std::shared_ptr<Executor>> CreateLocalExecutor(
    std::shared_ptr<IntrinsicHandlerSet> handler_set) {
  return CreateControlFlowExecutor(handler_set,
                                   GENC_TRY(CreateInlineExecutor(handler_set)));
}

absl::StatusOr<std::shared_ptr<Executor>> CreateDefaultLocalExecutor() {
  return CreateLocalExecutor(intrinsics::CreateCompleteHandlerSet({}));
}

}  // namespace genc
