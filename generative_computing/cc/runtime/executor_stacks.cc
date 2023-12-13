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

#include "generative_computing/cc/runtime/executor_stacks.h"

#include <memory>

#include "absl/status/statusor.h"
#include "generative_computing/cc/interop/backends/openai.h"
#include "generative_computing/cc/interop/backends/vertex_ai.h"
#include "generative_computing/cc/intrinsics/handler_sets.h"
#include "generative_computing/cc/modules/agents/react.h"
#include "generative_computing/cc/runtime/control_flow_executor.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/inline_executor.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/status_macros.h"

namespace generative_computing {

absl::StatusOr<std::shared_ptr<Executor>> CreateDefaultExecutor() {
  intrinsics::HandlerSetConfig config;

  // ReAct helper functions
  GENC_TRY(ReAct::SetCustomFunctions(config.custom_function_map));

  // OAI models
  GENC_TRY(OpenAI::SetInferenceMap(config.model_inference_map));

  // VertexAI models
  GENC_TRY(VertexAI::SetInferenceMap(config.model_inference_map));

  return CreateLocalExecutor(intrinsics::CreateCompleteHandlerSet(config));
}

absl::StatusOr<std::shared_ptr<Executor>> CreateLocalExecutor(
    std::shared_ptr<IntrinsicHandlerSet> handler_set) {
  return CreateControlFlowExecutor(handler_set,
                                   GENC_TRY(CreateInlineExecutor(handler_set)));
}

absl::StatusOr<std::shared_ptr<Executor>> CreateDefaultLocalExecutor() {
  return CreateLocalExecutor(intrinsics::CreateCompleteHandlerSet({}));
}

}  // namespace generative_computing
