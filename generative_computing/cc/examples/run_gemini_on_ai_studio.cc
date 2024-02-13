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
#include "generative_computing/cc/authoring/smart_chain.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/executor_stacks.h"
#include "generative_computing/cc/runtime/runner.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

// Demo calling Google AI Studio Gemini API as backend.
// To get API key follow https://ai.google.dev/
// bazle run generative_computing/cc/examples:run_gemini_on_ai_studio --
// --api_key=

ABSL_FLAG(std::string, api_key, "", "Gemini API Auth Token.");

namespace generative_computing {

absl::StatusOr<v0::Value> RunGeminiOnVertex(std::string json_request) {
  std::string api_key = absl::GetFlag(FLAGS_api_key);
  std::shared_ptr<Executor> executor = GENC_TRY(CreateDefaultExecutor());
  std::string end_point =
      "https://generativelanguage.googleapis.com/v1beta/models/"
      "gemini-pro:generateContent?key=" +
      api_key;
  // Makes a call to Gemini Pro on AI Studio.
  v0::Value rest_call = GENC_TRY(CreateRestCall(end_point));

  // Inja template can directly manipulate JSON string in the template.
  // Gemini response JSON chunks text into multiple parts. This template
  // 1. selects candidates[0] from response
  // 2. concatenates all parts in content into a single string.
  v0::Value format_json = GENC_TRY(CreateInjaTemplate(
      "{% if candidates %}{% for p in candidates.0.content.parts "
      "%}{{p.text}}{% endfor %}{%   endif %}"));

  // Chains the the rest call and format will form a chain that outputs
  // formatted string.
  SmartChain model_call_chain = SmartChain() | rest_call | format_json;

  v0::Value my_chain = GENC_TRY(model_call_chain.Build());
  Runner runner = GENC_TRY(Runner::Create(executor));
  v0::Value arg;
  arg.set_str(json_request);
  return runner.Run(my_chain, arg);
}
}  // namespace generative_computing

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);
  std::string json_request = R"pb(
    {
      "contents":
      [ {
        "parts":
        [ { "text": "Write a story about a magic backpack." }]
      }]
    }
  )pb";
  generative_computing::v0::Value output =
      generative_computing::RunGeminiOnVertex(json_request).value();
  std::cout << output.DebugString() << "\n";
  return 0;
}
