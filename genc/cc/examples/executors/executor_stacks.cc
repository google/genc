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

#include "genc/cc/examples/executors/executor_stacks.h"

#include <memory>
#include <utility>

#include "absl/base/call_once.h"
#include "absl/status/statusor.h"
#include "genc/cc/interop/backends/google_ai.h"
#include "genc/cc/intrinsics/handler_sets.h"
#include "genc/cc/modules/agents/react.h"
#include "genc/cc/modules/parsers/gemini_parser.h"
#include "genc/cc/modules/retrieval/local_cache.h"
#include "genc/cc/modules/tools/wolfram_alpha.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/executor_stacks.h"
#include "genc/cc/runtime/status_macros.h"

#include "genc/cc/interop/backends/llamacpp.h"

namespace genc {

// Stateful context that holds state (e.g. memory), which need to remain
// alive after initialization.
struct ExecutorStacksContext {
  ExecutorStacksContext(std::unique_ptr<LocalValueCache> local_cache)
      : local_cache_(std::move(local_cache)) {}
  std::unique_ptr<LocalValueCache> local_cache_;
};

namespace {
constexpr absl::string_view kLlamaCppModelUri = "/device/gemma";

static absl::once_flag context_init_flag;
static ExecutorStacksContext* exectuor_stacks_context = nullptr;
LlamaCpp* llama_cpp_client = nullptr;
constexpr int MAX_CACHE_SIZE_PER_KEY = 200;

// Initializes the executor stacks context.
static void InitExecutorStacksContext() {
  // Allocate memory for local_cache.
  auto local_cache = std::make_unique<LocalValueCache>(MAX_CACHE_SIZE_PER_KEY);
  // Transfer its ownership to a global context.
  exectuor_stacks_context = new ExecutorStacksContext(std::move(local_cache));
}

void SetLlamaCppModelInferenceHandler(intrinsics::HandlerSetConfig* config,
                                      absl::string_view model_uri) {
  config->model_inference_with_config_map[std::string(model_uri)] =
      [](v0::Intrinsic intrinsic, v0::Value arg) -> absl::StatusOr<v0::Value> {
    v0::Value model_inference;
    if (!llama_cpp_client) {
      // If the model hasn't been initialized, parse the config from
      // the intrinsic and create the model.
      const v0::Value& config =
          intrinsic.static_parameter().struct_().element(1);

      llama_cpp_client = new LlamaCpp();
      absl::Status status = llama_cpp_client->InitModel(config);
      if (!status.ok()) {
        return status;
      }
    }
    return llama_cpp_client->LlamaCppCall(arg);
  };
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

  SetLlamaCppModelInferenceHandler(&config, kLlamaCppModelUri);

  // Set access to local cache or other types of memory.
  GENC_TRY(SetCustomFunctionsForLocalValueCache(
      config.custom_function_map, *exectuor_stacks_context->local_cache_));

  return CreateLocalExecutor(intrinsics::CreateCompleteHandlerSet(config));
}

}  // namespace genc
