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

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "include/grpcpp/client_context.h"
#include "include/grpcpp/create_channel.h"
#include "include/grpcpp/security/credentials.h"
#include "include/grpcpp/support/status.h"
#include "proto/session/service_unary.grpc.pb.h"

ABSL_FLAG(std::string, server, "", "The address of the worker server.");

namespace genc {

absl::Status RunClient() {
  std::unique_ptr<::oak::session::v1::UnarySession::StubInterface> stub =
      oak::session::v1::UnarySession::NewStub(
          grpc::CreateChannel(
              absl::GetFlag(FLAGS_server),
              grpc::InsecureChannelCredentials()));
  grpc::ClientContext context;
  oak::session::v1::GetEndorsedEvidenceRequest request;
  oak::session::v1::GetEndorsedEvidenceResponse response;
  const grpc::Status status = stub->GetEndorsedEvidence(
      &context, request, &response);
  if (!status.ok()) {
    return absl::Status(
        static_cast<absl::StatusCode>(status.error_code()),
        status.error_message());
  }
  std::cout << "Server returned:\n" << response.DebugString() << "\n\n";
  return absl::OkStatus();
}

}  // namespace genc

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  absl::Status status = genc::RunClient();
  if (!status.ok()) {
    std::cout << "Client failed with status: " << status << "\n";
  }
  return 0;
}
