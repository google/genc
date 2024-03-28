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

#include <stdlib.h>

#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/statusor.h"
#include "genc/cc/authoring/constructor.h"
#include "genc/cc/examples/executors/executor_stacks.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/runner.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

// Demo calling VertexAI Model Garden as backend.
// To get appid, go to https://www.wolframalpha.com/api
// bazel run genc/cc/examples:run_wolfram -- --appid=<appid>

ABSL_FLAG(std::string, appid, "", "WolframAlpha AppId");

namespace genc {

absl::StatusOr<v0::Value> RunWolframAlpha(std::string query) {
  std::string appid = absl::GetFlag(FLAGS_appid);
  std::shared_ptr<Executor> executor = GENC_TRY(CreateDefaultExecutor());
  v0::Value wolfram_alpha_call = GENC_TRY(CreateWolframAlpha(appid));

  Runner runner = GENC_TRY(Runner::Create(executor));
  v0::Value arg;
  arg.set_str(query);
  return runner.Run(wolfram_alpha_call, arg);
}
}  // namespace genc

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  genc::v0::Value output =
      genc::RunWolframAlpha(
          "what is the result of 2^2-2-3+4*100")
          .value();
  std::cout << output.DebugString() << "\n";
  return 0;
}
