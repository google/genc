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
#include <future>  // NOLINT
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/intrinsics/model_inference.h"
#include "generative_computing/cc/intrinsics/prompt_template.h"
#include "generative_computing/cc/intrinsics/regex_partial_match.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/cc/runtime/threading.h"
#include "generative_computing/proto/v0/computation.pb.h"
#include "generative_computing/proto/v0/executor.pb.h"

namespace generative_computing {
namespace {

class ExecutorValue {
 public:
  explicit ExecutorValue(const std::shared_ptr<v0::Value>& value_pb)
      : value_(value_pb) {}

  ExecutorValue(const ExecutorValue& other) = default;
  ExecutorValue(ExecutorValue&& other) : value_(std::move(other.value_)) {}
  ExecutorValue& operator=(ExecutorValue&& other) {
    this->value_ = std::move(other.value_);
    return *this;
  }

  const v0::Value& value() const { return *value_; }

 private:
  ExecutorValue() = delete;

  std::shared_ptr<v0::Value> value_;
};

using ValueFuture = std::shared_future<absl::StatusOr<ExecutorValue>>;

// Executor that specializes in handling interactions with a model calls (i.e.,
// that only handles the `Model` computations as defined in the
// `computation.proto`, and routes to the appropriate model backends depending
// on the model declared).
class ModelExecutor : public ExecutorBase<ValueFuture> {
 public:
  explicit ModelExecutor(
      const intrinsics::ModelInference::InferenceMap& inference_map)
      : thread_pool_(nullptr), intrinsic_handlers_() {
    intrinsic_handlers_.AddHandler(
        new intrinsics::ModelInference(inference_map));
    intrinsic_handlers_.AddHandler(new intrinsics::PromptTemplate());
    intrinsic_handlers_.AddHandler(new intrinsics::RegexPartialMatch());
  }

  ~ModelExecutor() override { ClearTracked(); }

  absl::string_view ExecutorName() final {
    static constexpr absl::string_view kExecutorName = "ModelExecutor";
    return kExecutorName;
  }

  absl::StatusOr<ValueFuture> CreateExecutorValue(
      const v0::Value& val_pb) final {
    return ThreadRun(
        [val_pb]() -> absl::StatusOr<ExecutorValue> {
          return ExecutorValue(std::make_shared<v0::Value>(val_pb));
        },
        thread_pool_);
  }

  absl::Status Materialize(ValueFuture value_future, v0::Value* val_pb) final {
    ExecutorValue value = GENC_TRY(Wait(std::move(value_future)));
    if (val_pb != nullptr) {
      val_pb->CopyFrom(value.value());
    }
    return absl::OkStatus();
  }

  absl::StatusOr<ValueFuture> CreateCall(
      ValueFuture func_future, std::optional<ValueFuture> arg_future) final {
    if (!arg_future.has_value()) {
      return absl::InvalidArgumentError("An argument is always required.");
    }
    return ThreadRun(
        [function = std::move(func_future), argument = std::move(arg_future),
         this]() -> absl::StatusOr<ExecutorValue> {
          ExecutorValue fn = GENC_TRY(Wait(function));
          ExecutorValue arg = GENC_TRY(Wait(argument.value()));
          if (!fn.value().has_computation()) {
            return absl::InvalidArgumentError("Function is not a computation.");
          }
          if (!fn.value().computation().has_intrinsic()) {
            return absl::InvalidArgumentError(
                absl::StrCat("Unsupported function type: ",
                    fn.value().computation().DebugString()));
          }
          std::shared_ptr<v0::Value> result = std::make_shared<v0::Value>();
          GENC_TRY(intrinsic_handlers_.ExecuteCall(
              fn.value().computation().intrinsic(), arg.value(), nullptr,
              result.get()));
          return ExecutorValue(result);
        },
        thread_pool_);
  }

  absl::StatusOr<ValueFuture> CreateStruct(
      std::vector<ValueFuture> member_futures) final {
    return absl::UnimplementedError("Unimplemented");
  }

  absl::StatusOr<ValueFuture> CreateSelection(ValueFuture value_future,
                                              const uint32_t index) final {
    return absl::UnimplementedError("Unimplemented");
  }

 private:
  ThreadPool* const thread_pool_;
  IntrinsicHandlerSet intrinsic_handlers_;
};

}  // namespace

absl::StatusOr<std::shared_ptr<Executor>> CreateModelExecutor() {
  // TODO(b/295260921): Rename this to CreateEmptyModelExecutor or delete.
  // Reduce factory fns to eliminate complexity.
  return std::make_shared<ModelExecutor>(
      intrinsics::ModelInference::InferenceMap({}));
}

absl::StatusOr<std::shared_ptr<Executor>> CreateModelExecutorWithInferenceMap(
    const intrinsics::ModelInference::InferenceMap& inference_map) {
  return std::make_shared<ModelExecutor>(inference_map);
}

}  // namespace generative_computing
