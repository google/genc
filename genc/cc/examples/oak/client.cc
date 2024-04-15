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
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "genc/cc/interop/oak/client.h"
#include "genc/cc/runtime/status_macros.h"

// An example oak client.

namespace genc {
namespace {
}  // namespace

absl::Status RunClient() {
  // TODO(b/333410413): Do something.
  GENC_TRY(CreateOakClient());
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
