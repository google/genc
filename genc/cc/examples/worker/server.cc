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
#include "absl/strings/str_cat.h"
#include "genc/cc/runtime/executor_service.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/executor.grpc.pb.h"
#include "genc/proto/v0/executor.pb.h"
#include "include/grpcpp/security/server_credentials.h"
#include "include/grpcpp/server.h"
#include "include/grpcpp/server_builder.h"

// An example worker binary that hosts a gRPC service endpoint.
//
// Example usage:
//   bazel run genc/cc/examples/worker:server -- --port=<port>

ABSL_FLAG(int, port, 0, "The port to listed on.");

namespace genc {

absl::Status RunServer(int port) {
  std::string server_address = absl::StrCat("[::]:", port);
  std::shared_ptr<grpc::ServerCredentials> creds =
      grpc::InsecureServerCredentials();
  std::shared_ptr<v0::Executor::Service> service =
      GENC_TRY(CreateExecutorService());
  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, creds);
  builder.RegisterService(service.get());
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on: " << server_address << "\n";
  server->Wait();
  return absl::OkStatus();
}

}  // namespace genc

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  absl::Status status = genc::RunServer(
      absl::GetFlag(FLAGS_port));
  if (!status.ok()) {
    std::cout << "Server failed with status: " << status << "\n";
  }
  return 0;
}
