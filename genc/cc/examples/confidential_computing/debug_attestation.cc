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

// Using this tool to debug attestation report access issues:
//
// 1. Use the Confidential VM `debug` family to enable debug mode, passing the
//    flag `--image-family=confidential-space-debug` during VM creation.
//
// 2. SSH into the VM, and once in, enter an interactive shell in a TEE running
//    in it via `sudo ctr task exec -t --exec-id shell tee-container bash`.
//
// 3. Run this to locally fetch and print the attestation report from the UNIX
//    socket `/run/container_launcher/teeserver.sock`.

#include <iostream>
#include <string>

#include "absl/flags/parse.h"

#include "absl/flags/flag.h"
#include "absl/status/status.h"
#include "genc/cc/interop/confidential_computing/attestation.h"
#include "genc/cc/runtime/status_macros.h"
#include "tink/jwt/verified_jwt.h"

ABSL_FLAG(std::string, token, "", "The optional attestation token.");

namespace genc {
namespace {

absl::Status Run() {
  std::string token = absl::GetFlag(FLAGS_token);
  if (token.empty()) {
    token = GENC_TRY(interop::confidential_computing::GetAttestationToken());
    std::cout << "Token:\n" << token << "\n";
  }
  crypto::tink::VerifiedJwt verified_token =
      GENC_TRY(interop::confidential_computing::DecodeAttestationToken(token));
  std::cout << "Verified token:\n"
          << GENC_TRY(verified_token.GetJsonPayload()) << "\n";
  return absl::OkStatus();
}

}  // namespace
}  // namespace genc

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  absl::Status status = genc::Run();
  if (!status.ok()) {
    std::cout << "Error:\n" << status.ToString() << "\n";
    return -1;
  }
  return 0;
}
