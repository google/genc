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

#include "generative_computing/cc/intrinsics/handler_sets.h"

#include <memory>

#include "generative_computing/cc/intrinsics/breakable_chain.h"
#include "generative_computing/cc/intrinsics/conditional.h"
#include "generative_computing/cc/intrinsics/custom_function.h"
#include "generative_computing/cc/intrinsics/delegate.h"
#include "generative_computing/cc/intrinsics/fallback.h"
#include "generative_computing/cc/intrinsics/logger.h"
#include "generative_computing/cc/intrinsics/logical_not.h"
#include "generative_computing/cc/intrinsics/loop_chain_combo.h"
#include "generative_computing/cc/intrinsics/model_inference.h"
#include "generative_computing/cc/intrinsics/prompt_template.h"
#include "generative_computing/cc/intrinsics/regex_partial_match.h"
#include "generative_computing/cc/intrinsics/repeat.h"
#include "generative_computing/cc/intrinsics/while.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"

namespace generative_computing {
namespace intrinsics {

std::shared_ptr<IntrinsicHandlerSet> CreateCompleteHandlerSet(
    const HandlerSetConfig& config) {
  const std::shared_ptr<IntrinsicHandlerSet> handlers =
      std::make_shared<IntrinsicHandlerSet>();
  handlers->AddHandler(new intrinsics::BreakableChain());
  handlers->AddHandler(new intrinsics::Conditional());
  handlers->AddHandler(new intrinsics::Delegate(config.delegate_map));
  handlers->AddHandler(new intrinsics::Fallback());
  handlers->AddHandler(new intrinsics::Logger);
  handlers->AddHandler(
      new intrinsics::CustomFunction(config.custom_function_map));
  handlers->AddHandler(
      new intrinsics::ModelInference(config.model_inference_map));
  handlers->AddHandler(new intrinsics::LogicalNot());
  handlers->AddHandler(new intrinsics::PromptTemplate());
  handlers->AddHandler(new intrinsics::RegexPartialMatch());
  handlers->AddHandler(new intrinsics::Repeat());
  handlers->AddHandler(new intrinsics::While());
  handlers->AddHandler(new intrinsics::LoopChainCombo());
  return handlers;
}

}  // namespace intrinsics
}  // namespace generative_computing
