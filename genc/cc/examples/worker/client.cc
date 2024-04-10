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
#include "genc/proto/v0/executor.grpc.pb.h"
#include "genc/proto/v0/executor.pb.h"
#include "include/grpcpp/channel.h"
#include "include/grpcpp/client_context.h"
#include "include/grpcpp/create_channel.h"
#include "include/grpcpp/security/credentials.h"
#include "include/grpcpp/support/status.h"

// An example worker client that interacts with a worker server over gRPC.
//
// NOTE: This client has not been fully implemented yet.
//
// Example usage:
//   bazel run genc/cc/examples/worker:client -- \
//     --server=<address> --ir=<ir_file> --prompt=<prompt_string>

ABSL_FLAG(std::string, server, "", "The address of the worker server.");

namespace genc {

absl::Status RunClient(std::string server_address) {
  std::shared_ptr<grpc::ChannelCredentials> creds =
      grpc::InsecureChannelCredentials();
  std::shared_ptr<grpc::Channel> channel =
      grpc::CreateChannel(server_address, creds);
  std::unique_ptr<v0::Executor::StubInterface> executor_stub(
      v0::Executor::NewStub(channel));
  v0::CreateValueRequest request;
  v0::CreateValueResponse response;
  grpc::ClientContext client_context;
  grpc::Status status =
      executor_stub->CreateValue(&client_context, request, &response);
  if (!status.ok()) {
    return absl::InternalError(status.error_message());
  }
  return absl::OkStatus();
}

}  // namespace genc

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  absl::Status status = genc::RunClient(
      absl::GetFlag(FLAGS_server));
  if (!status.ok()) {
    std::cout << "Client failed with status: " << status << "\n";
  }
  return 0;
}
