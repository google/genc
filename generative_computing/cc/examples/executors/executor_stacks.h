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

#ifndef GENERATIVE_COMPUTING_CC_EXAMPLES_EXECUTORS_EXECUTOR_STACKS_H_
#define GENERATIVE_COMPUTING_CC_EXAMPLES_EXECUTORS_EXECUTOR_STACKS_H_

#include <memory>

#include "absl/status/statusor.h"
#include "generative_computing/cc/runtime/executor.h"

namespace generative_computing {

// Creates a default executors with out-of-box components initialized.
// Use this for convenience that can run all the examples, tutorials. For
// advanced uses, use CreateLocalExecutor to setup custom executors.
absl::StatusOr<std::shared_ptr<Executor>> CreateDefaultExecutor();
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_EXAMPLES_EXECUTORS_EXECUTOR_STACKS_H_
