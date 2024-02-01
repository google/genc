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
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/executor_stacks.h"
#include "generative_computing/cc/runtime/runner.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

// Demo calling VertexAI Gemini API as backend.
// https://cloud.google.com/vertex-ai/docs/generative-ai/model-reference/gemini
// To get API key run `gcloud auth print-access-token`
ABSL_FLAG(std::string, api_key, "", "Gemini API Auth Token.");

namespace generative_computing {

constexpr char kEndPoint[] =
    "https://us-central1-aiplatform.googleapis.com/v1/projects/25764558840/"
    "locations/us-central1/publishers/google/models/"
    "gemini-pro:streamGenerateContent";

absl::StatusOr<v0::Value> RunGeminiOnVertex(std::string json_request) {
  std::string api_key = absl::GetFlag(FLAGS_api_key);
  std::shared_ptr<Executor> executor = GENC_TRY(CreateDefaultExecutor());
  v0::Value rest_call = GENC_TRY(CreateRestCall(kEndPoint, api_key));

  Runner runner = GENC_TRY(Runner::Create(executor));
  v0::Value arg;
  arg.set_str(json_request);
  return runner.Run(rest_call, arg);
}
}  // namespace generative_computing

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string json_request = R"pb(
    {
      "contents": {
        "role": "user",
        "parts": { "text": "Give me a recipe for banana bread." },
      },
      "safety_settings": {
        "category": "HARM_CATEGORY_SEXUALLY_EXPLICIT",
        "threshold": "BLOCK_LOW_AND_ABOVE"
      },
      "generation_config": {
        "temperature": 0.9,
        "topP": 1.0,
        "maxOutputTokens": 2048
      }
    }
  )pb";
  generative_computing::v0::Value output =
      generative_computing::RunGeminiOnVertex(json_request).value();
  std::cout << output.DebugString() << "\n";
  return 0;
}
