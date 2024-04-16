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
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "genc/cc/base/read_file.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"
#include "genc/proto/v0/executor.pb.h"
#include "include/grpcpp/channel.h"
#include "include/grpcpp/create_channel.h"
#include "include/grpcpp/security/credentials.h"
#include "include/grpcpp/support/channel_arguments.h"
#include "cc/attestation/verification/attestation_verifier.h"
#include "cc/client/client.h"
#include "cc/transport/grpc_unary_transport.h"
#include "cc/transport/transport.h"
#include "proto/session/service_unary.grpc.pb.h"

ABSL_FLAG(std::string, server, "", "The address of the server.");
ABSL_FLAG(std::string, cert, "", "An optional path to the root certificate.");
ABSL_FLAG(std::string, target_override, "", "The expected target name.");

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

}  // namespace

absl::Status RunClient() {
  std::string server_address = absl::GetFlag(FLAGS_server);
  grpc::SslCredentialsOptions options;
  std::string cert = absl::GetFlag(FLAGS_cert);
  if (!cert.empty()) {
    options.pem_root_certs = ReadFile(cert);
  }
  std::shared_ptr<grpc::ChannelCredentials> creds =
      grpc::SslCredentials(options);
  std::shared_ptr<grpc::Channel> channel;
  std::string target_override = absl::GetFlag(FLAGS_target_override);
  if (target_override.empty()) {
    channel = grpc::CreateChannel(server_address, creds);
  } else {
    grpc::ChannelArguments args;
    args.SetSslTargetNameOverride(target_override);
    channel = grpc::CreateCustomChannel(server_address, creds, args);
  }
  std::unique_ptr<oak::session::v1::UnarySession::StubInterface> stub =
      oak::session::v1::UnarySession::NewStub(channel);
  std::unique_ptr<oak::transport::TransportWrapper> transport_wrapper =
      std::make_unique<oak::transport::GrpcUnaryTransport<
          oak::session::v1::UnarySession::StubInterface>>(stub.get());
  NoopAttestationVerifier attestation_verifier;
  std::unique_ptr<oak::client::OakClient> oak_client = GENC_TRY(
      oak::client::OakClient::Create(
          std::move(transport_wrapper), attestation_verifier));
  v0::ExecutorRequest request;
  request.mutable_create_value()->mutable_value()->set_str("Boo!");
  std::string serialized_request = request.SerializeAsString();
  std::string serialized_response =
      GENC_TRY(oak_client->Invoke(serialized_request));
  v0::ExecutorResponse response;
  if (!response.ParseFromString(serialized_response)) {
    return absl::InternalError("Failed to parse response.");
  }
  std::cout << "Server responsed:\n" << response.DebugString() << "\n";
  return absl::OkStatus();
}

}  // namespace genc

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  absl::Status status = genc::RunClient();
  if (!status.ok()) {
    std::cout << "Client failed with status:\n" << status << "\n";
  }
  return 0;
}
