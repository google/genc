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

#include "generative_computing/cc/runtime/model_executor.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/runtime/executor.h"

namespace generative_computing {
namespace {

class ExecutorValue {
 public:
  ExecutorValue() {}

  // TODO(b/295260921): Implement this. The values to be managed by this
  // executor would include arguments and results of model calls, as well as
  // model calls themselves.
};

// Executor that specializes in handling interactions with a model calls (i.e.,
// that only handles the `Model` computations as defined in the
// `computation.proto`, and routes to the appropriate model backends depending
// on the model declared).
class ModelExecutor : public ExecutorBase<ExecutorValue> {
 public:
  explicit ModelExecutor() {
    // TODO(b/295260921): Implement this. The executor needs to be able to take
    // some arguments to enable it to be configured to support some class of
    // model backends, some of which could be baked-in, but most of which would
    // need to be provisioned by the caller as a part of the runtime setup to
    // match the environment in which this runtime will be hosted.
  }

  ~ModelExecutor() override { ClearTracked(); }

  std::string_view ExecutorName() final {
    static constexpr std::string_view kExecutorName = "ModelExecutor";
    return kExecutorName;
  }

  absl::StatusOr<ExecutorValue> CreateExecutorValue(
      const v0::Value& val) final {
    // TODO(b/295260921): Implement this; the two types of values that would
    // be manually created include arguments to model inference calls as well
    // as model configurations themselves.
    return ExecutorValue();
  }

  absl::StatusOr<ExecutorValue> CreateCall(
      ExecutorValue function, std::optional<ExecutorValue> argument) final {
    // TODO(b/295260921): Implement this; the value shoudl represent a future
    // result of the model inference call;
    return ExecutorValue();
  }

  absl::StatusOr<ExecutorValue> CreateStruct(
      std::vector<ExecutorValue> members) final {
    // TODO(b/295260921): Implement this (for holding a set of call arguments).
    return ExecutorValue();
  }

  absl::StatusOr<ExecutorValue> CreateSelection(ExecutorValue value,
                                                const uint32_t index) final {
    // TODO(b/295260921): Implement this (for accessing a piece of the result).
    return ExecutorValue();
  }

  absl::Status Materialize(ExecutorValue value, v0::Value* val) final {
    // TODO(b/295260921): Implement this.
    return absl::OkStatus();
  }

  // TODO(b/295041601): Inherit the rest of core interfaces here, once defined.

  // TODO(b/295260921): Based on a prefix of the URI embedded in the `Model`,
  // route calls to an appropriate child executor or child component that
  // specializes in handling model calls for the corresponding class of models.
  // The set of supported models and child executors is something that should
  // come in as a parameter (since we want it to be a point of flexibility we
  // offer to whoever is handling runtime deployment and configuration).
  // In particular, we shoul be able to effectively route the calls to either
  // an on-device or a Cloud executor (and in general, there could be multiple
  // of each suported here within the same runtime stack).
};

}  // namespace

absl::StatusOr<std::shared_ptr<Executor>> CreateModelExecutor() {
  // TODO(b/295260921): Implement this; eventually this needs to take some
  // parameters to hook up this executor to the environment to enable it to
  // direct model calls to the appropriate backends.
  return std::make_shared<ModelExecutor>();
}

}  // namespace generative_computing
