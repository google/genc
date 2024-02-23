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

#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/authoring/constructor.h"
#include "generative_computing/cc/authoring/smart_chain.h"
#include "generative_computing/cc/examples/executors/executor_stacks.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/runner.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

// This example is meant to demonstrates that you can use combine reasoning and
// tools. It's not a realistic example in production, but it shows you one of
// the many ways how this is done.

// Reasoning loop behavior:
// Model break down the math into steps
// Each step it explains its thought, and formulate a math equation
// The equation is then calculated by Wolfram.
// These are then stored in a memory/context
// Repeat, proceed to next step of reasoning ...

// Sample Input:
// "Result of (square root of 4) + 3 to the power of 2 + 3 * (8 + 4) / 2 - 7

// Sample Output:
// Thought: I need to calculate the square root of 4 and cube 3 first.
// Action: Math[sqrt(4)]
// Observation: 2
// Thought: I need to cube 3 now.
// Action: Math[pow(3, 2)]
// Observation: 9
// Thought: Now I need to evaluate the expression.
// Action: Math[(2) + (9) + (3 * (8 + 4) / 2) - 7]
// Observation: 22
// Thought: Seems like I got the answer.
// Action: Finish[22]

ABSL_FLAG(std::string, api_key, "", "Gemini API Auth Token.");
ABSL_FLAG(std::string, appid, "", "WolframAlpha AppId");

namespace generative_computing {
constexpr char kInstructionTemplate[] =
    "Solve a question answering task with interleaving Thought, Action, "
    "Observation steps.\n"
    "Thought can reason about the current situation\n"
    "Action can be only two types:\n"
    "(1) Math[query], Useful for when you need to solve math problems.\n"
    "(2) Finish[answer], which returns the answer as a number terminates.\n"
    "Here's an example:\n"
    "Question: what is the result of power of 2 + 1?\n"
    "Thought: power of 2\n"
    "Action: Math[pow(2)]\n"
    "Observation: 4\n"
    "Thought: I can now process the answer.\n"
    "Action: Math[4+1]\n"
    "Observation: 5\n"
    "Thought: Seems like I got the answer.\n"
    "Action: Finish[5]\n"
    "Please do it step by step.\n"
    "Question: {question}\n";

absl::StatusOr<v0::Value> RunAgent(std::string question) {
  std::shared_ptr<Executor> executor = GENC_TRY(CreateDefaultExecutor());

  // Create modular computations, orders doesn't matter, these are just protos.
  // We employ lazy execution here. (eager is possible too).
  // Logger prints to terminal
  v0::Value log_it = GENC_TRY(CreateLogger());

  // Context keeps the interaction history with model.
  v0::Value read_context = GENC_TRY(CreateCustomFunction("/local_cache/read"));
  v0::Value add_to_context =
      GENC_TRY(CreateCustomFunction("/local_cache/write"));

  // Create a custom model call by utilizing Gemini behind REST endpoint.
  // To achieve the same as text-in-text-out model call
  // Please see run_gemini_on_ai_studio.cc and math_tool_agent.py
  std::string api_key = absl::GetFlag(FLAGS_api_key);
  std::string end_point =
      "https://generativelanguage.googleapis.com/v1beta/models/"
      "gemini-pro:generateContent?key=" +
      api_key;
  v0::Value rest_call = GENC_TRY(CreateRestCall(end_point));
  v0::Value str_to_json_request =
      GENC_TRY(CreateCustomFunction("/gemini_parser/wrap_text_as_input_json"));
  v0::Value extrat_top_candidate = GENC_TRY(
      CreateCustomFunction("/gemini_parser/get_top_candidate_as_text"));
  SmartChain model_call =
      SmartChain() | str_to_json_request | rest_call | extrat_top_candidate;

  // We use WolframAlpha as a Tool to solve simple math questions.
  std::string appid = absl::GetFlag(FLAGS_appid);
  v0::Value extract_result_from_wolfram = GENC_TRY(CreateInjaTemplate(
      "{% if queryresult.pods %}{{queryresult.pods.0.subpods.0.plaintext}}{% "
      "endif %}"));
  SmartChain solve_math =
      SmartChain() | CreateWolframAlpha(appid) | extract_result_from_wolfram;

  SmartChain reasoning_loop =
      SmartChain() | read_context | model_call |
      CreateCustomFunction("/react/parse_thought_action") | log_it |
      CreateRegexPartialMatch("Finish") | add_to_context |
      CreateCustomFunction("/react/extract_math_question") | solve_math |
      CreateCustomFunction("/react/format_observation") | log_it |
      add_to_context;

  // Set the max number of iterations of a reasoning loop.
  // This utilizes a RepeatedConditionalChain under the hood, which allows a
  // chain to be repeatedly executed until a break condition is met.
  reasoning_loop.SetNumIteration(8);

  SmartChain step_by_step_math_agent =
      SmartChain() | CreatePromptTemplate(kInstructionTemplate) |
      add_to_context | log_it | reasoning_loop;

  v0::Value computation = GENC_TRY(step_by_step_math_agent.Build());

  Runner runner = GENC_TRY(Runner::Create(executor));
  v0::Value arg;
  arg.set_str(question);
  return runner.Run(computation, arg);
}
}  // namespace generative_computing

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);

  std::string question =
      "what is the result of (square root of 4) + 3 to the power of 2 + 3 *"
      "(8 + 4) / 2 - 7";

  generative_computing::v0::Value output =
      generative_computing::RunAgent(question).value();
  return 0;
}
