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

#include <memory>

#include "absl/status/statusor.h"
#include "genc/cc/base/to_from_grpc_status.h"
#include "genc/cc/interop/oak/attestation_provider.h"
#include "genc/cc/interop/oak/server.h"
#include "genc/proto/v0/executor.grpc.pb.h"
#include "include/grpcpp/server_context.h"
#include "include/grpcpp/support/status.h"
#include "proto/session/service_unary.grpc.pb.h"

namespace genc {
namespace interop {
namespace oak {
namespace {

class OakService : public ::oak::session::v1::UnarySession::Service {
 public:
  explicit OakService(
      std::shared_ptr<v0::Executor::Service> executor_service,
      std::shared_ptr<AttestationProvider> attestation_provider)
      : executor_service_(executor_service),
        attestation_provider_(attestation_provider) {}

  ~OakService() override {}

  grpc::Status GetEndorsedEvidence(
      grpc::ServerContext* context,
      const ::oak::session::v1::GetEndorsedEvidenceRequest* request,
      ::oak::session::v1::GetEndorsedEvidenceResponse* response) override {
    absl::StatusOr<::oak::session::v1::EndorsedEvidence> endorsed_evidence =
        attestation_provider_->GetEndorsedEvidence();
    if (!endorsed_evidence.ok()) {
      return AbslToGrpcStatus(endorsed_evidence.status());
    } else {
      *response->mutable_endorsed_evidence() = endorsed_evidence.value();
      return grpc::Status::OK;
    }
  }

  grpc::Status Invoke(
      grpc::ServerContext* context,
      const ::oak::session::v1::InvokeRequest* request,
      ::oak::session::v1::InvokeResponse* response) override {
    return grpc::Status(
        grpc::StatusCode::UNIMPLEMENTED, "Invoke has not been implemented.");
  }

 private:
  const std::shared_ptr<v0::Executor::Service> executor_service_;
  const std::shared_ptr<AttestationProvider> attestation_provider_;
};

}  // namespace

absl::StatusOr<std::unique_ptr<::oak::session::v1::UnarySession::Service>>
CreateService(
    std::shared_ptr<v0::Executor::Service> executor_service,
    std::shared_ptr<AttestationProvider> attestation_provider) {
  return std::make_unique<OakService>(executor_service, attestation_provider);
}

}  // namespace oak
}  // namespace interop
}  // namespace genc
