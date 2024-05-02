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

#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "genc/cc/base/to_from_grpc_status.h"
#include "genc/cc/interop/oak/attestation_provider.h"
#include "genc/cc/interop/oak/server.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/executor.grpc.pb.h"
#include "genc/proto/v0/executor.pb.h"
#include "include/grpcpp/server_context.h"
#include "include/grpcpp/support/status.h"
#include "cc/crypto/common.h"
#include "cc/crypto/encryption_key.h"
#include "cc/crypto/server_encryptor.h"
#include "proto/session/service_unary.grpc.pb.h"

namespace genc {
namespace interop {
namespace oak {
namespace {

constexpr char kEmptyAssociatedData[] = "";

class OakService : public ::oak::session::v1::UnarySession::Service {
 public:
  static absl::StatusOr<std::unique_ptr<OakService>> Create(
      std::shared_ptr<v0::Executor::Service> executor_service,
      std::shared_ptr<AttestationProvider> attestation_provider,
      bool debug) {
    ::oak::crypto::EncryptionKeyProvider encryption_provider =
        GENC_TRY(::oak::crypto::EncryptionKeyProvider::Create());
    return std::make_unique<OakService>(
        executor_service,
        encryption_provider,
        std::move(attestation_provider),
        debug);
  }

  explicit OakService(
      std::shared_ptr<v0::Executor::Service> executor_service,
      const ::oak::crypto::EncryptionKeyProvider& encryption_provider,
      std::shared_ptr<AttestationProvider> attestation_provider,
      bool debug)
      : executor_service_(executor_service),
        encryption_provider_(encryption_provider),
        serialized_public_key_(encryption_provider_.GetSerializedPublicKey()),
        server_encryptor_(encryption_provider_),
        attestation_provider_(attestation_provider),
        debug_(debug) {
    if (debug_) {
      std::cout << "OakService public key:\n" << serialized_public_key_ << "\n";
    }
  }

  ~OakService() override {}

  grpc::Status GetEndorsedEvidence(
      grpc::ServerContext* context,
      const ::oak::session::v1::GetEndorsedEvidenceRequest* request,
      ::oak::session::v1::GetEndorsedEvidenceResponse* response) override {
    if (debug_) {
      std::cout << "OakService received the GetEndorsedEvidence() call\n";
    }
    absl::StatusOr<::oak::session::v1::EndorsedEvidence> endorsed_evidence =
        attestation_provider_->GetEndorsedEvidence(serialized_public_key_);
    if (!endorsed_evidence.ok()) {
      return AbslToGrpcStatus(
          absl::InternalError(absl::StrCat(
              "Attestation provider returned an error status: \"",
              endorsed_evidence.status().ToString(),
              "\".")));
    } else {
      *response->mutable_endorsed_evidence() = endorsed_evidence.value();
      return grpc::Status::OK;
    }
  }

  grpc::Status Invoke(
      grpc::ServerContext* context,
      const ::oak::session::v1::InvokeRequest* request,
      ::oak::session::v1::InvokeResponse* response) override {
    if (debug_) {
      std::cout << "OakService received the Invoke() call\n";
    }
    absl::StatusOr<::oak::crypto::DecryptionResult> decryption_result =
        server_encryptor_.Decrypt(request->encrypted_request());
    if (!decryption_result.ok()) {
      return AbslToGrpcStatus(
          absl::InternalError(absl::StrCat(
              "Request decryption failed with an error status: \"",
              decryption_result.status().ToString(),
              "\".")));
    }
    const std::string& serialized_request = decryption_result->plaintext;
    v0::ExecutorRequest executor_request;
    if (!executor_request.ParseFromString(serialized_request)) {
      return AbslToGrpcStatus(
          absl::InternalError(absl::StrCat(
              "Failed to parse the serialized request: \"",
              serialized_request,
              "\".")));
    }
    if (debug_) {
      std::cout << "OakService invoked with request:"
                << executor_request.DebugString() << "\n";
    }
    absl::StatusOr<v0::ExecutorResponse> executor_response =
        GetExecutorResponse(executor_request);
    if (!executor_response.ok()) {
      return AbslToGrpcStatus(
          absl::InternalError(absl::StrCat(
              "Failed to get the executor response: \"",
              executor_response.status().ToString(),
              "\".")));
    }
    if (debug_) {
      std::cout << "OakService returning response:"
                << executor_response->DebugString() << "\n";
    }
    absl::StatusOr<::oak::crypto::v1::EncryptedResponse> encrypted_response =
        server_encryptor_.Encrypt(
            executor_response->SerializeAsString(), kEmptyAssociatedData);
    if (!encrypted_response.ok()) {
      return AbslToGrpcStatus(
          absl::InternalError(absl::StrCat(
              "Response encryption failed with an error status: \"",
              encrypted_response.status().ToString(),
              "\".")));
    }
    *response->mutable_encrypted_response() = encrypted_response.value();
    return grpc::Status::OK;
  }

 private:
  absl::StatusOr<v0::ExecutorResponse> GetExecutorResponse(
      const v0::ExecutorRequest& executor_request) {
    v0::ExecutorResponse executor_response;
    grpc::Status grpc_status;
    if (executor_request.has_create_value()) {
      grpc_status = executor_service_->CreateValue(
          nullptr,
          &executor_request.create_value(),
          executor_response.mutable_create_value());
    } else if (executor_request.has_create_call()) {
      grpc_status = executor_service_->CreateCall(
          nullptr,
          &executor_request.create_call(),
          executor_response.mutable_create_call());
    } else if (executor_request.has_create_struct()) {
      grpc_status = executor_service_->CreateStruct(
          nullptr,
          &executor_request.create_struct(),
          executor_response.mutable_create_struct());
    } else if (executor_request.has_create_selection()) {
      grpc_status = executor_service_->CreateSelection(
          nullptr,
          &executor_request.create_selection(),
          executor_response.mutable_create_selection());
    } else if (executor_request.has_materialize()) {
      grpc_status = executor_service_->Materialize(
          nullptr,
          &executor_request.materialize(),
          executor_response.mutable_materialize());
    } else if (executor_request.has_dispose()) {
      grpc_status = executor_service_->Dispose(
          nullptr,
          &executor_request.dispose(),
          executor_response.mutable_dispose());
    } else {
      return absl::InternalError(absl::StrCat(
          "Unsupported type of executor request: \"",
          executor_request.DebugString(),
          "\"."));
    }
    if (!grpc_status.ok()) {
      return GrpcToAbslStatus(grpc_status);
    } else {
      return executor_response;
    }
  }

  const std::shared_ptr<v0::Executor::Service> executor_service_;
  ::oak::crypto::EncryptionKeyProvider encryption_provider_;
  const std::string serialized_public_key_;
  ::oak::crypto::ServerEncryptor server_encryptor_;
  const std::shared_ptr<AttestationProvider> attestation_provider_;
  const bool debug_;
};

}  // namespace

absl::StatusOr<std::unique_ptr<::oak::session::v1::UnarySession::Service>>
CreateService(
    std::shared_ptr<v0::Executor::Service> executor_service,
    std::shared_ptr<AttestationProvider> attestation_provider,
    bool debug) {
  return OakService::Create(executor_service, attestation_provider, debug);
}

}  // namespace oak
}  // namespace interop
}  // namespace genc
