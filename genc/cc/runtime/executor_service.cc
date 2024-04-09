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

#include "absl/status/statusor.h"
#include "genc/proto/v0/executor.grpc.pb.h"
#include "genc/proto/v0/executor.pb.h"
#include "include/grpcpp/server_context.h"
#include "include/grpcpp/support/status.h"

namespace genc {

// An implementation of the `Executor` service defined in executor.proto
class ExecutorService : public v0::Executor::Service {
 public:
  ExecutorService() {}

  ~ExecutorService() override {}

  grpc::Status CreateValue(grpc::ServerContext* context,
                           const v0::CreateValueRequest* request,
                           v0::CreateValueResponse* response) override {
    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not implemented.");
  }

  grpc::Status CreateCall(grpc::ServerContext* context,
                          const v0::CreateCallRequest* request,
                          v0::CreateCallResponse* response) override {
    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not implemented.");
  }

  grpc::Status CreateStruct(grpc::ServerContext* context,
                            const v0::CreateStructRequest* request,
                            v0::CreateStructResponse* response) override {
    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not implemented.");
  }

  grpc::Status CreateSelection(grpc::ServerContext* context,
                               const v0::CreateSelectionRequest* request,
                               v0::CreateSelectionResponse* response) override {
    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not implemented.");
  }

  grpc::Status Materialize(grpc::ServerContext* context,
                           const v0::MaterializeRequest* request,
                           v0::MaterializeResponse* response) override {
    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not implemented.");
  }

  grpc::Status Dispose(grpc::ServerContext* context,
                       const v0::DisposeRequest* request,
                       v0::DisposeResponse* response) override {
    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not implemented.");
  }
};

absl::StatusOr<std::shared_ptr<v0::Executor::Service>> CreateExecutorService() {
  return std::make_shared<ExecutorService>();
}

}  // namespace genc
