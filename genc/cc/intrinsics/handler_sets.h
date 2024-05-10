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

#ifndef GENC_CC_INTRINSICS_HANDLER_SETS_H_
#define GENC_CC_INTRINSICS_HANDLER_SETS_H_

#include <memory>
#include <vector>

#include "genc/cc/interop/networking/http_client_interface.h"
#include "genc/cc/intrinsics/custom_function.h"
#include "genc/cc/intrinsics/delegate.h"
#include "genc/cc/intrinsics/model_inference.h"
#include "genc/cc/intrinsics/model_inference_with_config.h"
#include "genc/cc/runtime/intrinsic_handler.h"

namespace genc {
namespace intrinsics {

struct HandlerSetConfig {
  Delegate::RunnerMap delegate_map;
  // TODO(b/325090417): Consolidate model_inference_map and
  // model_inference_with_config_map into one map.
  ModelInference::InferenceMap model_inference_map;
  ModelInferenceWithConfig::InferenceMap model_inference_with_config_map;
  CustomFunction::FunctionMap custom_function_map;
  std::vector<const IntrinsicHandler*> custom_intrinsics_list;

  // An optional HTTP client interface for use by handlers that need to issue
  // network HTTP requests (NULL by default, leaving the handlers to use CURL
  // by default).
  // NOTE: At this time, most handlers are not modified to use this inrerface.
  // New handlers should use this interface if possible.
  std::shared_ptr<interop::networking::HttpClientInterface>
      http_client_interface;
};

// Construct a new handler set.
std::shared_ptr<IntrinsicHandlerSet> CreateCompleteHandlerSet(
    const HandlerSetConfig& config);

}  // namespace intrinsics
}  // namespace genc

#endif  // GENC_CC_INTRINSICS_HANDLER_SETS_H_
