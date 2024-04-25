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
#include "genc/cc/interop/confidential_computing/attestation.h"

int main(int argc, char* argv[]) {
  auto provider = genc::interop::confidential_computing::
      CreateAttestationProvider();
  if (!provider.ok()) {
    std::cout << "Error while creating an attestation provider.\n";
    return -1;
  }
  auto evidence = provider.value()->GetEndorsedEvidence();
  if (!evidence.ok()) {
    std::cout << "Error while calling the attestation provider:\n"
              << evidence.status().ToString() << "\n";
    return -1;
  }
  std::cout << "Endorsed evidence:\n" << evidence->DebugString() << "\n";
  return 0;
}
