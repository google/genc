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
#include <utility>

#include "absl/base/call_once.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/interop/backends/google_ai.h"
#include "generative_computing/cc/intrinsics/handler_sets.h"
#include "generative_computing/cc/modules/agents/react.h"
#include "generative_computing/cc/modules/parsers/gemini_parser.h"
#include "generative_computing/cc/modules/retrieval/local_cache.h"
#include "generative_computing/cc/modules/tools/wolfram_alpha.h"
#include "generative_computing/cc/runtime/control_flow_executor.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/inline_executor.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/status_macros.h"

namespace generative_computing {

// Stateful context that holds state (e.g. memory), which need to remain
// alive after initialization.
struct ExecutorStacksContext {
  ExecutorStacksContext(std::unique_ptr<LocalValueCache> local_cache)
      : local_cache_(std::move(local_cache)) {}
  std::unique_ptr<LocalValueCache> local_cache_;
};

namespace {
static absl::once_flag context_init_flag;
static ExecutorStacksContext* exectuor_stacks_context = nullptr;
constexpr int MAX_CACHE_SIZE_PER_KEY = 200;

// Initializes the executor stacks context.
static void InitExecutorStacksContext() {
  // Allocate memory for local_cache.
  auto local_cache = std::make_unique<LocalValueCache>(MAX_CACHE_SIZE_PER_KEY);
  // Transfer its ownership to a global context.
  exectuor_stacks_context = new ExecutorStacksContext(std::move(local_cache));
}
}  // namespace

absl::StatusOr<std::shared_ptr<Executor>> CreateDefaultExecutor() {
  // Init the context only once.
  absl::call_once(context_init_flag, InitExecutorStacksContext);

  intrinsics::HandlerSetConfig config;
  // ReAct helper functions.
  GENC_TRY(ReAct::SetCustomFunctions(config.custom_function_map));

  // Gemini Parsers
  GENC_TRY(GeminiParser::SetCustomFunctions(config.custom_function_map));

  // Enable Wolfram Alpha intrinsic
  config.custom_intrinsics_list.push_back(new intrinsics::WolframAlpha());

  // Set model inference for Gemini backends.
  GENC_TRY(GoogleAI::SetInferenceMap(config.model_inference_with_config_map));

  // Set access to local cache or other types of memory.
  GENC_TRY(SetCustomFunctionsForLocalValueCache(
      config.custom_function_map, *exectuor_stacks_context->local_cache_));

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
