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

#include "genc/cc/intrinsics/handler_sets.h"

#include <memory>
#include <vector>

#include "genc/cc/intrinsics/breakable_chain.h"
#include "genc/cc/intrinsics/conditional.h"
#include "genc/cc/intrinsics/confidential_computation.h"
#include "genc/cc/intrinsics/custom_function.h"
#include "genc/cc/intrinsics/delegate.h"
#include "genc/cc/intrinsics/fallback.h"
#include "genc/cc/intrinsics/inja_template.h"
#include "genc/cc/intrinsics/logger.h"
#include "genc/cc/intrinsics/logical_not.h"
#include "genc/cc/intrinsics/model_inference.h"
#include "genc/cc/intrinsics/model_inference_with_config.h"
#include "genc/cc/intrinsics/parallel_map.h"
#include "genc/cc/intrinsics/prompt_template.h"
#include "genc/cc/intrinsics/regex_partial_match.h"
#include "genc/cc/intrinsics/repeat.h"
#include "genc/cc/intrinsics/repeated_conditional_chain.h"
#include "genc/cc/intrinsics/rest_call.h"
#include "genc/cc/intrinsics/serial_chain.h"
#include "genc/cc/intrinsics/while.h"
#include "genc/cc/runtime/intrinsic_handler.h"

namespace genc {
namespace intrinsics {

std::shared_ptr<IntrinsicHandlerSet> CreateCompleteHandlerSet(
    const HandlerSetConfig& config) {
  const std::shared_ptr<IntrinsicHandlerSet> handlers =
      std::make_shared<IntrinsicHandlerSet>();
  handlers->AddHandler(new intrinsics::BreakableChain());
  handlers->AddHandler(new intrinsics::SerialChain());
  handlers->AddHandler(new intrinsics::Conditional());
  handlers->AddHandler(new intrinsics::ConfidentialComputation(
      config.http_client_interface));
  handlers->AddHandler(new intrinsics::Delegate(config.delegate_map));
  handlers->AddHandler(new intrinsics::Fallback());
  handlers->AddHandler(new intrinsics::Logger);
  handlers->AddHandler(new intrinsics::InjaTemplate());
  handlers->AddHandler(
      new intrinsics::CustomFunction(config.custom_function_map));
  handlers->AddHandler(
      new intrinsics::ModelInference(config.model_inference_map));
  handlers->AddHandler(
      new intrinsics::ModelInferenceWithConfig(
          config.model_inference_with_config_map));
  handlers->AddHandler(new intrinsics::ParallelMap());
  handlers->AddHandler(new intrinsics::LogicalNot());
  handlers->AddHandler(new intrinsics::PromptTemplate());
  handlers->AddHandler(new intrinsics::PromptTemplateWithParameters());
  handlers->AddHandler(new intrinsics::RegexPartialMatch());
  handlers->AddHandler(new intrinsics::Repeat());
  handlers->AddHandler(new intrinsics::RestCall());
  handlers->AddHandler(new intrinsics::While());
  handlers->AddHandler(new intrinsics::RepeatedConditionalChain());

  // Add all custom intrinsics provided via the handler config.
  for (int i = 0; i < config.custom_intrinsics_list.size(); ++i) {
    handlers->AddHandler(config.custom_intrinsics_list[i]);
  }
  return handlers;
}

}  // namespace intrinsics
}  // namespace genc
