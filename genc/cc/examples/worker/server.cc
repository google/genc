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
#include "genc/cc/examples/executors/executor_stacks.h"
#include "genc/cc/modules/worker/run_server.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/status_macros.h"

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

ABSL_FLAG(int, port, 0, "The port to listen on.");
ABSL_FLAG(std::string, key, "", "Path to the private key for SSL/TLS.");
ABSL_FLAG(std::string, cert, "", "Path to the certificate for SSL/TLS.");
ABSL_FLAG(bool, oak, false, "Whether to use project Oak for communication.");

namespace genc {

absl::Status RunServer() {
  std::shared_ptr<Executor> executor = GENC_TRY(CreateDefaultExecutor());
  modules::worker::RunServerOptions options;
  options.server_address = absl::StrCat("[::]:", absl::GetFlag(FLAGS_port));
  if (!absl::GetFlag(FLAGS_cert).empty() ||
      !absl::GetFlag(FLAGS_key).empty()) {
    options.channel_type = modules::worker::RunServerOptions::SSL;
    options.ssl_cert_path = absl::GetFlag(FLAGS_cert);
    options.ssl_cert_path = absl::GetFlag(FLAGS_key);
  }
  options.use_oak = absl::GetFlag(FLAGS_oak);
  return modules::worker::RunServer(executor, options);
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
