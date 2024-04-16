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

#include "genc/cc/runtime/executor_service.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_cat.h"
#include "genc/cc/base/to_from_grpc_status.h"
#include "genc/cc/runtime/executor.h"
#include "genc/proto/v0/executor.grpc.pb.h"
#include "genc/proto/v0/executor.pb.h"
#include "include/grpcpp/server_context.h"
#include "include/grpcpp/support/status.h"

namespace genc {
namespace {

v0::ValueRef ValueIdToRef(ValueId id) {
  v0::ValueRef value_ref;
  value_ref.set_id(absl::StrCat(id));
  return value_ref;
}

absl::StatusOr<ValueId> RefToValueId(v0::ValueRef ref) {
  ValueId value_id;
  if (absl::SimpleAtoi(ref.id(), &value_id)) {
    return value_id;
  } else {
    return absl::InvalidArgumentError(absl::StrCat(
        "Invalid value reference: ", ref.id()));
  }
}

}  // namespace

// An implementation of the `Executor` service defined in executor.proto that
// maps incoming calls to an underlying implementation of C++ `Executor` API.
class ExecutorService : public v0::Executor::Service {
 public:
  explicit ExecutorService(std::shared_ptr<Executor> executor)
      : executor_(executor) {}

  ~ExecutorService() override {}

  grpc::Status CreateValue(grpc::ServerContext* context,
                           const v0::CreateValueRequest* request,
                           v0::CreateValueResponse* response) override {
    absl::StatusOr<OwnedValueId> val = executor_->CreateValue(request->value());
    if (!val.ok()) {
      return AbslToGrpcStatus(val.status());
    }
    *response->mutable_value_ref() = ValueIdToRef(val->ref());
    val->forget();
    return grpc::Status::OK;
  }

  grpc::Status CreateCall(grpc::ServerContext* context,
                          const v0::CreateCallRequest* request,
                          v0::CreateCallResponse* response) override {
    absl::StatusOr<ValueId> func = RefToValueId(request->function_ref());
    if (!func.ok()) {
      return AbslToGrpcStatus(func.status());
    }
    std::optional<ValueId> optional_arg;
    if (request->has_argument_ref()) {
      absl::StatusOr<ValueId> arg = RefToValueId(request->argument_ref());
      if (!arg.ok()) {
        return AbslToGrpcStatus(arg.status());
      }
      optional_arg = arg.value();
    }
    absl::StatusOr<OwnedValueId> result =
        executor_->CreateCall(func.value(), optional_arg);
    if (!result.ok()) {
      return AbslToGrpcStatus(result.status());
    }
    *response->mutable_result_ref() = ValueIdToRef(result->ref());
    result->forget();
    return grpc::Status::OK;
  }

  grpc::Status CreateStruct(grpc::ServerContext* context,
                            const v0::CreateStructRequest* request,
                            v0::CreateStructResponse* response) override {
    std::vector<ValueId> elements;
    elements.reserve(request->element_ref().size());
    for (const v0::ValueRef& val_ref : request->element_ref()) {
      absl::StatusOr<ValueId> val = RefToValueId(val_ref);
      if (!val.ok()) {
        return AbslToGrpcStatus(val.status());
      }
      elements.push_back(val.value());
    }
    absl::StatusOr<OwnedValueId> result = executor_->CreateStruct(elements);
    if (!result.ok()) {
      return AbslToGrpcStatus(result.status());
    }
    *response->mutable_struct_ref() = ValueIdToRef(result->ref());
    result->forget();
    return grpc::Status::OK;
  }

  grpc::Status CreateSelection(grpc::ServerContext* context,
                               const v0::CreateSelectionRequest* request,
                               v0::CreateSelectionResponse* response) override {
    absl::StatusOr<ValueId> source = RefToValueId(request->source_ref());
    if (!source.ok()) {
      return AbslToGrpcStatus(source.status());
    }
    absl::StatusOr<OwnedValueId> selection =
        executor_->CreateSelection(source.value(), request->index());
    if (!selection.ok()) {
      return AbslToGrpcStatus(selection.status());
    }
    *response->mutable_selection_ref() = ValueIdToRef(selection->ref());
    selection->forget();
    return grpc::Status::OK;
  }

  grpc::Status Materialize(grpc::ServerContext* context,
                           const v0::MaterializeRequest* request,
                           v0::MaterializeResponse* response) override {
    absl::StatusOr<ValueId> val = RefToValueId(request->value_ref());
    if (!val.ok()) {
      return AbslToGrpcStatus(val.status());
    }
    absl::Status status =
        executor_->Materialize(val.value(), response->mutable_value());
    return AbslToGrpcStatus(status);
  }

  grpc::Status Dispose(grpc::ServerContext* context,
                       const v0::DisposeRequest* request,
                       v0::DisposeResponse* response) override {
    std::vector<std::string> error_strings;
    std::vector<ValueId> values;
    values.reserve(request->value_ref().size());
    for (const v0::ValueRef& value_ref : request->value_ref()) {
      absl::StatusOr<ValueId> val = RefToValueId(value_ref);
      if (!val.ok()) {
        error_strings.push_back(val.status().ToString());
      } else {
        absl::Status status = executor_->Dispose(val.value());
        if (!status.ok()) {
           error_strings.push_back(status.ToString());
        }
      }
    }
    if (!error_strings.empty()) {
      return AbslToGrpcStatus(
          absl::InternalError(absl::StrJoin(error_strings, "\n")));
    }
    return grpc::Status::OK;
  }

 private:
  const std::shared_ptr<Executor> executor_;
};

absl::StatusOr<std::shared_ptr<v0::Executor::Service>> CreateExecutorService(
    std::shared_ptr<Executor> executor) {
  return std::make_shared<ExecutorService>(executor);
}

}  // namespace genc
