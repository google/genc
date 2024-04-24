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

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "genc/cc/base/read_file.h"
#include "genc/cc/interop/confidential_computing/attestation.h"
#include "genc/cc/interop/oak/server.h"
#include "genc/cc/modules/worker/run_server.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/executor_service.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/executor.grpc.pb.h"
#include "genc/proto/v0/executor.pb.h"
#include "include/grpcpp/security/server_credentials.h"
#include "include/grpcpp/server.h"
#include "include/grpcpp/server_builder.h"
#include "proto/session/service_unary.grpc.pb.h"

namespace genc {
namespace modules {
namespace worker {
namespace {

absl::StatusOr<std::shared_ptr<grpc::ServerCredentials>> GetCreds(
    const RunServerOptions& opt) {
  if (opt.channel_type == RunServerOptions::SSL) {
     grpc::SslServerCredentialsOptions ssl_options;
     ssl_options.pem_key_cert_pairs.push_back({
          ReadFile(opt.ssl_key_path),
          ReadFile(opt.ssl_cert_path)});
     ssl_options.pem_root_certs = ReadFile(opt.ssl_cert_path);
    return grpc::SslServerCredentials(ssl_options);
  }
  if (opt.channel_type == RunServerOptions::INSECURE) {
     return grpc::InsecureServerCredentials();
  }
  return absl::InvalidArgumentError("Unsupported channel type");
}

}  // namespace

absl::Status RunServer(
    std::shared_ptr<Executor> executor, const RunServerOptions& options) {
  std::shared_ptr<grpc::ServerCredentials> creds = GENC_TRY(GetCreds(options));
  std::shared_ptr<v0::Executor::Service> executor_service =
      GENC_TRY(CreateExecutorService(executor));
  grpc::ServerBuilder builder;
  builder.AddListeningPort(options.server_address, creds);
  std::shared_ptr<oak::session::v1::UnarySession::Service> oak_service;
  if (options.use_oak) {
    if (options.debug) {
      std::cout << "Exposing Oak's UnarySession service.\n";
    }
    oak_service = GENC_TRY(interop::oak::CreateService(
        executor_service,
        GENC_TRY(
            interop::confidential_computing::CreateAttestationProvider()),
        options.debug));
    builder.RegisterService(oak_service.get());
  } else {
    if (options.debug) {
      std::cout << "Exposing the regular Executor service.\n";
    }
    builder.RegisterService(executor_service.get());
  }
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  if (options.debug) {
    std::cout << "Server listening on: " << options.server_address << "\n";
  }
  server->Wait();
  if (options.debug) {
    std::cout << "Server exiting.\n";
  }
  return absl::OkStatus();
}

}  // namespace worker
}  // namespace modules
}  // namespace genc

