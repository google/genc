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

#ifndef GENERATIVE_COMPUTING_CC_RUNTIME_RUNNER_H_
#define GENERATIVE_COMPUTING_CC_RUNTIME_RUNNER_H_

#include <memory>
#include <utility>

#include "absl/status/statusor.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

// Runner that runs arbitrary computation proto.
class Runner final {
 public:
  // Creates a Runner with a default local executor.
  static absl::StatusOr<Runner> CreateDefault();

  // Creates a Runner with custom executor.
  static absl::StatusOr<Runner> Create(std::shared_ptr<Executor> executor);

  // Runs the computation & returns the resulting value.
  absl::StatusOr<v0::Value> Run(v0::Value computation, v0::Value arg);

 private:
  Runner(std::shared_ptr<Executor> executor) : executor_(executor) {}

  std::shared_ptr<Executor> executor_;
};

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_RUNNER_H_
