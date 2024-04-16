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

#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "genc/cc/base/read_file.h"
#include "genc/cc/examples/executors/executor_stacks.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/executor_service.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/executor.grpc.pb.h"
#include "genc/proto/v0/executor.pb.h"
#include "include/grpcpp/security/server_credentials.h"
#include "include/grpcpp/server.h"
#include "include/grpcpp/server_builder.h"

// An example worker binary that hosts a gRPC service endpoint.
//
// NOTE: This server has not been fully implemented yet.
//
// Example usage:
//   bazel run genc/cc/examples/worker:server -- --port=<port>
//
// For secure communication using SSL/TLS, you need to additionally pass the
// private key and certificate. You can generate a self-signed certificate
// for testing, e.g., using the `openssl` tool, as follows:
//   openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem \
//     -sha256 -days 3650 -nodes
// Assuming you ran the above in the `/tmp` directory, and the key and cert
// are there, you can then start the server on port 10000 as follows:
//   bazel run genc/cc/examples/worker:server -- \
//     --port=10000 --cert=/tmp/cert.pem --key=/tmp/key.pem

ABSL_FLAG(int, port, 0, "The port to listed on.");
ABSL_FLAG(std::string, key, "", "Path to the private key for SSL/TLS.");
ABSL_FLAG(std::string, cert, "", "Path to the certificate for SSL/TLS.");

namespace genc {

std::string CreateServerAddress() {
  return absl::StrCat("[::]:", absl::GetFlag(FLAGS_port));
}

std::shared_ptr<grpc::ServerCredentials> CreateServerCredentials() {
  std::string key = absl::GetFlag(FLAGS_key);
  if (key.empty()) {
    return grpc::InsecureServerCredentials();
  }
  std::string cert = absl::GetFlag(FLAGS_cert);
  grpc::SslServerCredentialsOptions options;
  options.pem_key_cert_pairs.push_back({ReadFile(key), ReadFile(cert)});
  options.pem_root_certs = ReadFile(cert);
  return grpc::SslServerCredentials(options);
}

absl::StatusOr<std::shared_ptr<Executor>> CreateExecutor() {
  return CreateDefaultExecutor();
}

absl::Status RunServer() {
  std::string server_address = CreateServerAddress();
  std::shared_ptr<grpc::ServerCredentials> creds = CreateServerCredentials();
  std::shared_ptr<v0::Executor::Service> service =
      GENC_TRY(CreateExecutorService(GENC_TRY(CreateExecutor())));
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
  absl::Status status = genc::RunServer();
  if (!status.ok()) {
    std::cout << "Server failed with status: " << status << "\n";
  }
  return 0;
}
