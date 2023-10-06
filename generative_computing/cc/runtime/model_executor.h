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

#ifndef GENERATIVE_COMPUTING_CC_RUNTIME_MODEL_EXECUTOR_H_
#define GENERATIVE_COMPUTING_CC_RUNTIME_MODEL_EXECUTOR_H_

#include <functional>
#include <memory>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/runtime/executor.h"

namespace generative_computing {

// Returns an executor that specializes in handling interactions with models.
absl::StatusOr<std::shared_ptr<Executor>> CreateModelExecutor();

// Returns an executor that handles interaction with models, but also accepts
// a mapping from model_uri to a function that does the model inference. Use
// this method to inject support for additional models, beyond the base models
// that are supported at runtime.
// TODO(b/299566821): generalize for repeated multimodal response.
typedef std::function<absl::StatusOr<std::string>(absl::string_view)>
    InferenceFn;
absl::StatusOr<std::shared_ptr<Executor>> CreateModelExecutorWithInferenceMap(
    const absl::flat_hash_map<std::string, InferenceFn>& inference_map);

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_MODEL_EXECUTOR_H_
