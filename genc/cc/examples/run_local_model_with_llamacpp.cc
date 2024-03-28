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
constexpr absl::string_view kModelUri = "/device/local_model";

namespace genc {

absl::StatusOr<v0::Value> RunLocalModelWithLlamaCpp(
    std::shared_ptr<LlamaCpp> llamacpp, std::string prompt) {
  intrinsics::HandlerSetConfig config;
  GENC_TRY(llamacpp->SetInferenceMap(config.model_inference_map, kModelUri));
  std::shared_ptr<Executor> executor = GENC_TRY(
      CreateLocalExecutor(intrinsics::CreateCompleteHandlerSet(config)));
  v0::Value model_call = GENC_TRY(CreateModelInference(kModelUri));

  Runner runner = GENC_TRY(Runner::Create(executor));
  v0::Value arg = GENC_TRY(llamacpp->CreateRequest(prompt));

  return runner.Run(model_call, arg);
}
}  // namespace genc

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string model_path = absl::GetFlag(FLAGS_model_path);
  int num_threads = absl::GetFlag(FLAGS_num_threads);
  int max_tokens = absl::GetFlag(FLAGS_max_tokens);
  std::string prompt = absl::GetFlag(FLAGS_prompt);
  std::shared_ptr<genc::LlamaCpp> llamacpp =
      std::make_shared<genc::LlamaCpp>();
  const auto& config = genc::CreateLlamaCppConfig(
      model_path, num_threads, max_tokens);
  absl::Status status = llamacpp->InitModel(config.value());
  if (!status.ok()) {
    std::cout << "Couldn't create model (" << model_path << ")\n";
    return 1;
  }

  genc::v0::Value output =
      genc::RunLocalModelWithLlamaCpp(llamacpp, prompt).value();
  std::cout << "GenC Response: " << output.DebugString() << "\n";
  return 0;
}
