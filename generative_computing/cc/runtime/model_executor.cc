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
#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/intrinsics/intrinsics.h"
#include "generative_computing/cc/runtime/intrinsics/prompt_template.h"
#include "generative_computing/cc/runtime/intrinsics/regex_partial_match.h"
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
      const absl::flat_hash_map<std::string, InferenceFn>& inference_map)
      : inference_map_(inference_map),
        thread_pool_(nullptr),
        intrinsic_handlers_() {
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
      return absl::InvalidArgumentError("Model calls require an argument.");
    }
    return ThreadRun(
        [function = std::move(func_future), argument = std::move(arg_future),
         this]() -> absl::StatusOr<ExecutorValue> {
          ExecutorValue fn = GENC_TRY(Wait(function));
          ExecutorValue arg = GENC_TRY(Wait(argument.value()));
          if (!fn.value().has_computation()) {
            return absl::InvalidArgumentError("Function is not a computation.");
          }
          // TODO(b/295015950): Delete non-intrinsic branches as a part of the
          // intrinsic support-based cleanup.
          if (!fn.value().computation().has_model() &&
              !fn.value().computation().has_intrinsic()) {
            return absl::InvalidArgumentError(
                absl::StrCat("Unsupported function type: ",
                    fn.value().computation().DebugString()));
          }
          if (!arg.value().has_str()) {
            return absl::InvalidArgumentError("Argument is not a string.");
          }

          // TODO(b/295015950): Delete non-intrinsic branches as a part of the
          // intrinsic support-based cleanup.
          std::shared_ptr<v0::Value> result = std::make_shared<v0::Value>();
          if (fn.value().computation().has_model()) {
            GENC_TRY(Generate(fn.value().computation().model().model_id().uri(),
                              arg.value().str(), result->mutable_str()));
          } else {
            // Must therefore be an intrinsic.
            GENC_TRY(CallIntrinsic(
                fn.value().computation().intrinsic(),
                arg.value(),
                result.get()));
          }
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
  absl::Status CallIntrinsic(
      const v0::Intrinsic& intrinsic, const v0::Value& arg, v0::Value* result) {
    // TODO(b/295015950): Eventually delete all the special-casing here.
    // intrinsic support-based cleanup.
    if (intrinsic.uri() == intrinsics::kModelInference) {
      return CallIntrinsicModelInference(intrinsic, arg, result);
    } else {
      return intrinsic_handlers_.ExecuteCall(intrinsic, arg, nullptr, result);
    }
  }

  absl::Status CallIntrinsicModelInference(const v0::Intrinsic& intrinsic,
                                           const v0::Value& arg,
                                           v0::Value* result) {
    if (intrinsic.static_parameter_size() != 1) {
      return absl::InvalidArgumentError("Wrong number of arguments.");
    } else {
      return Generate(intrinsic.static_parameter(0).value().str(), arg.str(),
                      result->mutable_str());
    }
  }

  // TODO(b/299566821): generalize for repeated multimodal response.
  absl::Status Generate(const absl::string_view& model_uri,
                        const absl::string_view arg, std::string* output) {
    if (model_uri == "test_model") {
      output->assign(absl::StrCat(
          "This is an output from a test model in response to \"", arg, "\"."));
      return absl::OkStatus();
    }
    if (inference_map_.contains(model_uri)) {
      *output = GENC_TRY(inference_map_.at(model_uri)(arg));
      return absl::OkStatus();
    }

    // TODO(b/295260921): Based on a prefix of the URI embedded in the `Model`,
    // route calls to an appropriate child executor or child component that
    // specializes in handling calls for the corresponding class of models.
    // The set of supported models and child executors is something that should
    // come in as a parameter (since we want it to be a point of flexibility we
    // offer to whoever is handling runtime deployment and configuration).
    // In particular, we should be able to effectively route the calls to either
    // an on-device or a Cloud executor (and in general, there could be multiple
    // of each supported here within the same runtime stack).
    return absl::UnimplementedError(
        absl::StrCat("Unsupported model: ", model_uri));
  }

  const absl::flat_hash_map<std::string, InferenceFn> inference_map_;
  ThreadPool* const thread_pool_;
  IntrinsicHandlerSet intrinsic_handlers_;
};

}  // namespace

absl::StatusOr<std::shared_ptr<Executor>> CreateModelExecutor() {
  // TODO(b/295260921): Rename this to CreateEmptyModelExecutor or delete.
  // Reduce factory fns to eliminate complexity.
  return std::make_shared<ModelExecutor>(
      absl::flat_hash_map<std::string, InferenceFn>({}));
}

absl::StatusOr<std::shared_ptr<Executor>> CreateModelExecutorWithInferenceMap(
    const absl::flat_hash_map<std::string, InferenceFn>& inference_map) {
  return std::make_shared<ModelExecutor>(inference_map);
}

}  // namespace generative_computing
