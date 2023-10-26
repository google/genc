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

#ifndef GENERATIVE_COMPUTING_CC_INTRINSICS_HANDLER_SETS_H_
#define GENERATIVE_COMPUTING_CC_INTRINSICS_HANDLER_SETS_H_

#include <memory>

#include "generative_computing/cc/intrinsics/delegate.h"
#include "generative_computing/cc/intrinsics/model_inference.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"

namespace generative_computing {
namespace intrinsics {

struct HandlerSetConfig {
  Delegate::RunnerMap delegate_map;
  ModelInference::InferenceMap model_inference_map;
};

// Construct a new handler set.
std::shared_ptr<IntrinsicHandlerSet> CreateCompleteHandlerSet(
    const HandlerSetConfig& config);

}  // namespace intrinsics
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_INTRINSICS_HANDLER_SETS_H_
