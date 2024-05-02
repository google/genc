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

#include <chrono>  // NOLINT
#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/parse.h"

#include "absl/status/status.h"
#include "genc/cc/interop/confidential_computing/attestation.h"
#include "genc/cc/runtime/status_macros.h"
#include "cc/crypto/encryption_key.h"

namespace genc {
namespace {

absl::Status Run() {
  ::oak::crypto::EncryptionKeyProvider encryption_provider =
      GENC_TRY(oak::crypto::EncryptionKeyProvider::Create());
  const std::string serialized_public_key =
      encryption_provider.GetSerializedPublicKey();
  std::shared_ptr<interop::confidential_computing::AttestationProvider>
      attestation_provider = GENC_TRY(
          interop::confidential_computing::CreateAttestationProvider(true));
  oak::session::v1::EndorsedEvidence endorsed_evidence = GENC_TRY(
      attestation_provider->GetEndorsedEvidence(serialized_public_key));
  interop::confidential_computing::WorkloadProvenance provenance;
  std::shared_ptr<interop::confidential_computing::AttestationVerifier>
      attestation_verifier = GENC_TRY(
          interop::confidential_computing::CreateAttestationVerifier(
              provenance, true));
  oak::attestation::v1::AttestationResults verification_results =
      GENC_TRY(attestation_verifier->Verify(
          std::chrono::system_clock::now(),
          endorsed_evidence.evidence(),
          endorsed_evidence.endorsements()));
  std::cout << "Results:\n" << verification_results.DebugString() << "\n";
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
