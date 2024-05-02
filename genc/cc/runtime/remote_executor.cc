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

#include <algorithm>
#include <cstdint>
#include <future>  // NOLINT
#include <memory>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "genc/cc/base/to_from_grpc_status.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/remote_executor.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/cc/runtime/threading.h"
#include "genc/proto/v0/computation.pb.h"
#include "genc/proto/v0/executor.grpc.pb.h"
#include "genc/proto/v0/executor.pb.h"
#include "include/grpcpp/client_context.h"
#include "include/grpcpp/support/status.h"

namespace genc {
namespace {

using ExecutorStub = v0::Executor::StubInterface;

class ExecutorValue {
 public:
  ExecutorValue(
      std::shared_ptr<ExecutorStub> executor_stub,
      v0::ValueRef value_ref)
      : executor_stub_(executor_stub),
        value_ref_(std::move(value_ref)) {}

  ~ExecutorValue() {
    ThreadRun([executor_stub = executor_stub_, value_ref = value_ref_] {
      v0::DisposeRequest request;
      v0::DisposeResponse response;
      grpc::ClientContext context;
      *request.add_value_ref() = value_ref;
      const grpc::Status status =
          executor_stub->Dispose(&context, request, &response);
      if (!status.ok()) {
        // Silently ignore for now...
      }
    });
  }

  const v0::ValueRef& ref() const { return value_ref_; }

 private:
  const std::shared_ptr<ExecutorStub> executor_stub_;
  const v0::ValueRef value_ref_;
};

using ValueFuture =
    std::shared_future<absl::StatusOr<std::shared_ptr<ExecutorValue>>>;

class RemoteExecutor : public ExecutorBase<ValueFuture> {
 public:
  explicit RemoteExecutor(
      std::unique_ptr<ExecutorStub> stub,
      std::shared_ptr<ThreadPool> thread_pool = nullptr)
      : executor_stub_(stub.release()),
        thread_pool_(thread_pool) {}

  ~RemoteExecutor() override = default;

  std::string_view ExecutorName() final {
    static constexpr std::string_view kExecutorName = "RemoteExecutor";
    return kExecutorName;
  }

  absl::StatusOr<ValueFuture> CreateExecutorValue(
      const v0::Value& val_pb) final {
    return ThreadRun(
        [val_pb,
        this,
        this_keepalive = shared_from_this()]()
        -> absl::StatusOr<std::shared_ptr<ExecutorValue>> {
          grpc::ClientContext client_context;
          v0::CreateValueRequest request;
          v0::CreateValueResponse response;
          *request.mutable_value() = val_pb;
          grpc::Status status = executor_stub_->CreateValue(
              &client_context, request, &response);
          GENC_TRY(GrpcToAbslStatus(status));
          return std::make_shared<ExecutorValue>(
              executor_stub_, std::move(response.value_ref()));
        },
        thread_pool_);
  }

  absl::Status Materialize(ValueFuture value_future, v0::Value* val_pb) final {
    std::shared_ptr<ExecutorValue> value_ref = GENC_TRY(Wait(value_future));
    grpc::ClientContext client_context;
    v0::MaterializeRequest request;
    v0::MaterializeResponse response;
    *request.mutable_value_ref() = value_ref->ref();
    grpc::Status status = executor_stub_->Materialize(
        &client_context, request, &response);
    *val_pb = std::move(*response.mutable_value());
    return GrpcToAbslStatus(status);
  }

  absl::StatusOr<ValueFuture> CreateCall(
      ValueFuture func_future, std::optional<ValueFuture> arg_future) final {
    return ThreadRun(
        [func = std::move(func_future),
         arg = std::move(arg_future),
         this,
        this_keepalive = shared_from_this()]()
        -> absl::StatusOr<std::shared_ptr<ExecutorValue>> {
          std::shared_ptr<ExecutorValue> func_value = GENC_TRY(Wait(func));
          grpc::ClientContext context;
          v0::CreateCallRequest request;
          v0::CreateCallResponse response;
          *request.mutable_function_ref() = func_value->ref();
          if (arg.has_value()) {
            std::shared_ptr<ExecutorValue> arg_value =
                GENC_TRY(Wait(arg.value()));
            *request.mutable_argument_ref() = arg_value->ref();
          }
          grpc::Status status = executor_stub_->CreateCall(
              &context, request, &response);
          GENC_TRY(GrpcToAbslStatus(status));
          return std::make_shared<ExecutorValue>(
              executor_stub_, std::move(response.result_ref()));
        },
        thread_pool_);
  }

  absl::StatusOr<ValueFuture> CreateStruct(
      std::vector<ValueFuture> member_futures) final {
    return ThreadRun(
        [elements = std::move(member_futures),
        this,
        this_keepalive = shared_from_this()]()
        -> absl::StatusOr<std::shared_ptr<ExecutorValue>> {
          grpc::ClientContext context;
          v0::CreateStructRequest request;
          v0::CreateStructResponse response;
          for (const ValueFuture& element : elements) {
            std::shared_ptr<ExecutorValue> element_value =
                GENC_TRY(Wait(element));
            *request.add_element_ref() = element_value->ref();
          }
          grpc::Status status = executor_stub_->CreateStruct(
              &context, request, &response);
          GENC_TRY(GrpcToAbslStatus(status));
          return std::make_shared<ExecutorValue>(
              executor_stub_, std::move(response.struct_ref()));
        },
        thread_pool_);
  }

  absl::StatusOr<ValueFuture> CreateSelection(
      ValueFuture value_future, const uint32_t index) final {
    return ThreadRun(
        [source = std::move(value_future),
        index = index,
        this,
        this_keepalive = shared_from_this()]()
        -> absl::StatusOr<std::shared_ptr<ExecutorValue>> {
          std::shared_ptr<ExecutorValue> source_value = GENC_TRY(Wait(source));
          grpc::ClientContext client_context;
          v0::CreateSelectionRequest request;
          v0::CreateSelectionResponse response;
          *request.mutable_source_ref() = source_value->ref();
          request.set_index(index);
          grpc::Status status = executor_stub_->CreateSelection(
              &client_context, request, &response);
          GENC_TRY(GrpcToAbslStatus(status));
          return std::make_shared<ExecutorValue>(
              executor_stub_, std::move(response.selection_ref()));
        },
        thread_pool_);
  }

 private:
  const std::shared_ptr<ExecutorStub> executor_stub_;
  const std::shared_ptr<ThreadPool> thread_pool_;
};

}  // namespace

absl::StatusOr<std::shared_ptr<Executor>> CreateRemoteExecutor(
    std::unique_ptr<v0::Executor::StubInterface> executor_stub,
    std::shared_ptr<ThreadPool> thread_pool) {
  return std::make_shared<RemoteExecutor>(
      std::move(executor_stub), std::move(thread_pool));
}

}  // namespace genc
