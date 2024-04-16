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

#include <chrono>  // NOLINT
#include <memory>
#include <string>
#include <utility>

#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "genc/cc/base/to_from_grpc_status.h"
#include "genc/cc/interop/oak/client.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"
#include "genc/proto/v0/executor.grpc.pb.h"
#include "genc/proto/v0/executor.pb.h"
#include "include/grpcpp/channel.h"
#include "include/grpcpp/client_context.h"
#include "include/grpcpp/support/async_unary_call.h"
#include "include/grpcpp/support/status.h"
#include "cc/attestation/verification/attestation_verifier.h"
#include "cc/client/client.h"
#include "cc/transport/grpc_unary_transport.h"
#include "cc/transport/transport.h"
#include "proto/session/service_unary.grpc.pb.h"

namespace genc {
namespace {

// Temporarily implemented here due to issues with InsecureAttestationVerifier's
// problematic dependency on libcppbor that trips the OSS build.
// TODO(b/333410413): Replace this with Oak's InsecureAttestationVerifier, or
// something else that isn't a noop.
class NoopAttestationVerifier
    : public oak::attestation::verification::AttestationVerifier {
 public:
  absl::StatusOr<oak::attestation::v1::AttestationResults> Verify(
      std::chrono::time_point<std::chrono::system_clock> now,
      const ::oak::attestation::v1::Evidence& evidence,
      const ::oak::attestation::v1::Endorsements& endorsements) const override {
    oak::attestation::v1::AttestationResults attestation_results;
    attestation_results.set_status(
        oak::attestation::v1::AttestationResults::STATUS_SUCCESS);
    return attestation_results;
  }
};

class OakClient : public v0::Executor::StubInterface {
 public:
  static absl::StatusOr<std::unique_ptr<OakClient>> Create(
      std::shared_ptr<grpc::Channel> channel) {
    std::unique_ptr<oak::session::v1::UnarySession::StubInterface> stub =
      oak::session::v1::UnarySession::NewStub(channel);
    std::unique_ptr<oak::transport::TransportWrapper> transport_wrapper =
        std::make_unique<oak::transport::GrpcUnaryTransport<
            oak::session::v1::UnarySession::StubInterface>>(stub.get());
    std::unique_ptr<NoopAttestationVerifier> attestation_verifier;
    std::unique_ptr<oak::client::OakClient> oak_client = GENC_TRY(
        oak::client::OakClient::Create(
            std::move(transport_wrapper), *attestation_verifier));
    return absl::WrapUnique(
        new OakClient(std::move(attestation_verifier), std::move(oak_client)));
  }

  grpc::Status CreateValue(
      grpc::ClientContext* context,
      const v0::CreateValueRequest& request,
      v0::CreateValueResponse* response) override {
    v0::ExecutorRequest executor_request;
    v0::ExecutorResponse executor_response;
    *executor_request.mutable_create_value() = request;
    absl::Status status = Call(executor_request, &executor_response);
    if (!status.ok()) {
      return AbslToGrpcStatus(status);
    }
    if (!executor_response.has_create_value()) {
      return grpc::Status(
          grpc::StatusCode::INTERNAL,
          "Received a mismatching type of response.");
    }
    *response = executor_response.create_value();
    return grpc::Status::OK;
  }

  grpc::Status CreateCall(
      grpc::ClientContext* context,
      const v0::CreateCallRequest& request,
      v0::CreateCallResponse* response) override {
    v0::ExecutorRequest executor_request;
    v0::ExecutorResponse executor_response;
    *executor_request.mutable_create_call() = request;
    absl::Status status = Call(executor_request, &executor_response);
    if (!status.ok()) {
      return AbslToGrpcStatus(status);
    }
    if (!executor_response.has_create_call()) {
      return grpc::Status(
          grpc::StatusCode::INTERNAL,
          "Received a mismatching type of response.");
    }
    *response = executor_response.create_call();
    return grpc::Status::OK;
  }

  grpc::Status CreateStruct(
      grpc::ClientContext* context,
      const v0::CreateStructRequest& request,
      v0::CreateStructResponse* response) override {
    v0::ExecutorRequest executor_request;
    v0::ExecutorResponse executor_response;
    *executor_request.mutable_create_struct() = request;
    absl::Status status = Call(executor_request, &executor_response);
    if (!status.ok()) {
      return AbslToGrpcStatus(status);
    }
    if (!executor_response.has_create_struct()) {
      return grpc::Status(
          grpc::StatusCode::INTERNAL,
          "Received a mismatching type of response.");
    }
    *response = executor_response.create_struct();
    return grpc::Status::OK;
  }

  grpc::Status CreateSelection(
      grpc::ClientContext* context,
      const v0::CreateSelectionRequest& request,
      v0::CreateSelectionResponse* response) override {
    v0::ExecutorRequest executor_request;
    v0::ExecutorResponse executor_response;
    *executor_request.mutable_create_selection() = request;
    absl::Status status = Call(executor_request , &executor_response);
    if (!status.ok()) {
      return AbslToGrpcStatus(status);
    }
    if (!executor_response.has_create_selection()) {
      return grpc::Status(
          grpc::StatusCode::INTERNAL,
          "Received a mismatching type of response.");
    }
    *response = executor_response.create_selection();
    return grpc::Status::OK;
  }

  grpc::Status Materialize(
      grpc::ClientContext* context,
      const v0::MaterializeRequest& request,
      v0::MaterializeResponse* response) override {
    v0::ExecutorRequest executor_request;
    v0::ExecutorResponse executor_response;
    *executor_request.mutable_materialize() = request;
    absl::Status status = Call(executor_request, &executor_response);
    if (!status.ok()) {
      return AbslToGrpcStatus(status);
    }
    if (!executor_response.has_materialize()) {
      return grpc::Status(
          grpc::StatusCode::INTERNAL,
          "Received a mismatching type of response.");
    }
    *response = executor_response.materialize();
    return grpc::Status::OK;
  }

  grpc::Status Dispose(
      grpc::ClientContext* context,
      const v0::DisposeRequest& request,
      v0::DisposeResponse* response) override {
    v0::ExecutorRequest executor_request;
    v0::ExecutorResponse executor_response;
    *executor_request.mutable_dispose() = request;
    absl::Status status = Call(executor_request , &executor_response);
    if (!status.ok()) {
      return AbslToGrpcStatus(status);
    }
    if (!executor_response.has_dispose()) {
      return grpc::Status(
          grpc::StatusCode::INTERNAL,
          "Received a mismatching type of response.");
    }
    *response = executor_response.dispose();
    return grpc::Status::OK;
  }

  grpc::ClientAsyncResponseReaderInterface<v0::CreateValueResponse>*
      AsyncCreateValueRaw(
          grpc::ClientContext* context,
          const v0::CreateValueRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::CreateValueResponse>*
      PrepareAsyncCreateValueRaw(
          grpc::ClientContext* context,
          const v0::CreateValueRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::CreateCallResponse>*
      AsyncCreateCallRaw(
          grpc::ClientContext* context,
          const v0::CreateCallRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::CreateCallResponse>*
      PrepareAsyncCreateCallRaw(
          grpc::ClientContext* context,
          const v0::CreateCallRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::CreateStructResponse>*
      AsyncCreateStructRaw(
          grpc::ClientContext* context,
          const v0::CreateStructRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::CreateStructResponse>*
      PrepareAsyncCreateStructRaw(
          grpc::ClientContext* context,
          const v0::CreateStructRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::CreateSelectionResponse>*
      AsyncCreateSelectionRaw(
          grpc::ClientContext* context,
          const v0::CreateSelectionRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::CreateSelectionResponse>*
      PrepareAsyncCreateSelectionRaw(
          grpc::ClientContext* context,
          const v0::CreateSelectionRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::MaterializeResponse>*
      AsyncMaterializeRaw(
          grpc::ClientContext* context,
          const v0::MaterializeRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::MaterializeResponse>*
      PrepareAsyncMaterializeRaw(
          grpc::ClientContext* context,
          const v0::MaterializeRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::DisposeResponse>*
      AsyncDisposeRaw(
          grpc::ClientContext* context,
          const v0::DisposeRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

  grpc::ClientAsyncResponseReaderInterface<v0::DisposeResponse>*
      PrepareAsyncDisposeRaw(
          grpc::ClientContext* context,
          const v0::DisposeRequest& request,
          grpc::CompletionQueue* cq) override { return nullptr; }

 protected:
  OakClient(
      std::unique_ptr<NoopAttestationVerifier> attestation_verifier,
      std::unique_ptr<oak::client::OakClient> oak_client)
      : attestation_verifier_(std::move(attestation_verifier)),
        oak_client_(std::move(oak_client)) {}

 private:
  absl::Status Call(
      const v0::ExecutorRequest& request,
      v0::ExecutorResponse* response) {
    std::string serialized_request = request.SerializeAsString();
    absl::StatusOr<std::string> serialized_response =
        GENC_TRY(oak_client_.value()->Invoke(serialized_request));
    if (!response->ParseFromString(serialized_response.value())) {
      return absl::InternalError("Failed to parse the serialized response.");
    }
    return absl::OkStatus();
  }

  const std::unique_ptr<NoopAttestationVerifier> attestation_verifier_;
  const absl::StatusOr<std::unique_ptr<oak::client::OakClient>> oak_client_;
};

}  // namespace

absl::StatusOr<std::unique_ptr<v0::Executor::StubInterface>> CreateOakClient(
    std::shared_ptr<grpc::Channel> channel) {
  return OakClient::Create(channel);
}

}  // namespace genc
