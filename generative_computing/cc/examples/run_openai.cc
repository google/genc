/* Copyright 2023, The Generative Computing Authors.

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
#include "generative_computing/cc/interop/backends/openai.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/executor_stacks.h"
#include "generative_computing/cc/runtime/runner.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"
// Demo calling OpenAI ChatGPT as backend.
ABSL_FLAG(std::string, api_key, "", "OpenAI API key.");

namespace generative_computing {

constexpr char kEndPoint[] =
    "https://api.openai.com/v1/engines/davinci/completions";

absl::StatusOr<v0::Value> RunOpenAI(std::string api_key,
                                    std::string json_request) {
  std::shared_ptr<Executor> executor = GENC_TRY(CreateDefaultExecutor());
  v0::Value model_call = GENC_TRY(CreateModelInference("/openai/chatgpt"));
  Runner runner = GENC_TRY(Runner::Create(executor));
  v0::Value arg =
      GENC_TRY(OpenAI::CreateRequest(api_key, kEndPoint, json_request));
  return runner.Run(model_call, arg);
}
}  // namespace generative_computing

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string json_request = R"pb(
    { "prompt": "tell me a story!", "max_tokens": 50 }
  )pb";
  std::string api_key = absl::GetFlag(FLAGS_api_key);
  generative_computing::v0::Value output =
      generative_computing::RunOpenAI(api_key, json_request).value();
  std::cout << output.DebugString() << "\n";
  return 0;
}
