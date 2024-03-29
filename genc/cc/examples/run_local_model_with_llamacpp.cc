/* Copyright 2024, The GenC Authors.

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
#include "genc/cc/interop/backends/llamacpp.h"
#include "genc/cc/intrinsics/handler_sets.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/executor_stacks.h"
#include "genc/cc/runtime/runner.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

ABSL_FLAG(std::string, model_path, "", "Model Path (LlamaCPP Compatible)");
ABSL_FLAG(int, num_threads, 4, "Num threads for local model");
ABSL_FLAG(int, max_tokens, 64, "Max tokens for local model (excluding prompt)");
ABSL_FLAG(std::string, prompt, "Tell me a joke", "Prompt");

constexpr absl::string_view kModelUri = "/device/gemma";

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string model_path = absl::GetFlag(FLAGS_model_path);
  int num_threads = absl::GetFlag(FLAGS_num_threads);
  int max_tokens = absl::GetFlag(FLAGS_max_tokens);
  std::string prompt = absl::GetFlag(FLAGS_prompt);

  std::shared_ptr<genc::Executor> executor =
      genc::CreateDefaultExecutor().value();
  genc::v0::Value model_call = genc::CreateModelInferenceWithConfig(
      kModelUri,
      genc::CreateLlamaCppConfig(
          model_path, num_threads, max_tokens).value()).value();
  genc::v0::Value arg;
  arg.set_str(prompt);
  genc::Runner runner = genc::Runner::Create(model_call, executor).value();
  genc::v0::Value output = runner.Run(arg).value();
  std::cout << "GenC Response: " << output.DebugString() << "\n";
  return 0;
}
