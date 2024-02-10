/* Copyright 2024, The Generative Computing Authors.

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
#include "generative_computing/cc/authoring/constructor.h"
#include "generative_computing/cc/interop/backends/llamacpp.h"
#include "generative_computing/cc/intrinsics/handler_sets.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/executor_stacks.h"
#include "generative_computing/cc/runtime/runner.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

ABSL_FLAG(std::string, model_path, "", "Model Path (LlamaCPP Compatible)");
ABSL_FLAG(int, num_threads, 4, "Num threads for local model");
ABSL_FLAG(int, max_tokens, 64, "Max tokens for local model (excluding prompt)");
ABSL_FLAG(std::string, prompt, "Tell me a joke", "Prompt");
constexpr absl::string_view kModelUri = "/device/local_model";

namespace generative_computing {

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
}  // namespace generative_computing

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string model_path = absl::GetFlag(FLAGS_model_path);
  int num_threads = absl::GetFlag(FLAGS_num_threads);
  int max_tokens = absl::GetFlag(FLAGS_max_tokens);
  std::string prompt = absl::GetFlag(FLAGS_prompt);
  std::shared_ptr<generative_computing::LlamaCpp> llamacpp =
      std::make_shared<generative_computing::LlamaCpp>();
  absl::Status status =
      llamacpp->InitModel(model_path, num_threads, max_tokens);
  if (!status.ok()) {
    std::cout << "Couldn't create model (" << model_path << ")\n";
    return 1;
  }

  generative_computing::v0::Value output =
      generative_computing::RunLocalModelWithLlamaCpp(llamacpp, prompt).value();
  std::cout << "GenC Response: " << output.DebugString() << "\n";
  return 0;
}