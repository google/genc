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
  // Creates a Runner with a default local executor and no computation.
  // The computation will have to be provided in the `Run` call.
  static absl::StatusOr<Runner> CreateDefault();

  // Creates a Runner with a custom executor and no computation.
  // The computation will have to be provided in the `Run` call.
  static absl::StatusOr<Runner> Create(std::shared_ptr<Executor> executor);

  // Creates a Runner with a default executor and a computation.
  // The subsequent `Run` calls will all use the computation provided here.
  static absl::StatusOr<Runner> Create(v0::Value computation);

  // Creates a Runner with a custom executor and a computation.
  // The subsequent `Run` calls will all use the computation provided here.
  static absl::StatusOr<Runner> Create(v0::Value computation,
                                       std::shared_ptr<Executor> executor);

  // Runs the computation supplied in the constructor and returns the
  // resulting value. Note that if no computation was provided in the
  // constructor, this call will fail.
  absl::StatusOr<v0::Value> Run(v0::Value arg);

  // Runs the computation supplied below as an argument and returns the
  // resulting value. Note that if a computation was already provided in the
  // constructor, this call will fail.
  absl::StatusOr<v0::Value> Run(v0::Value computation, v0::Value arg);

 private:
  Runner(std::shared_ptr<v0::Value> computation,
         std::shared_ptr<Executor> executor)
            : computation_or_null_(computation),
              executor_(executor) {}

  absl::StatusOr<v0::Value> RunInternal(v0::Value computation, v0::Value arg);

  std::shared_ptr<v0::Value> computation_or_null_;
  std::shared_ptr<Executor> executor_;
};

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_RUNNER_H_
