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

#include "genc/cc/modules/agents/react.h"

#include <cstddef>
#include <iostream>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "genc/cc/intrinsics/custom_function.h"
#include "re2/re2.h"
namespace genc {

absl::StatusOr<v0::Value> ReAct::ExtractPythonCode(v0::Value input) {
  RE2::Options options;
  options.set_dot_nl(true);
  RE2 code_block_regex(R"(```python(.*?)```)", options);

  std::string output;
  if (RE2::PartialMatch(input.str(), code_block_regex, &output)) {
  } else {
    std::cout << "No Python code block found in the input." << std::endl;
  }

  std::string runnable_code_suffix =
      ""
      "result = compute()\n"
      "print(result)\n";

  v0::Value output_pb;
  output_pb.set_str(output + runnable_code_suffix);
  return output_pb;
}

absl::StatusOr<v0::Value> ReAct::ExtractJsCode(v0::Value input) {
  RE2::Options options;
  options.set_dot_nl(true);
  RE2 code_block_regex(R"(```(?:javascript|js)?(.*?)```)", options);

  std::string output;
  if (RE2::PartialMatch(input.str(), code_block_regex, &output)) {
  } else {
    std::cout << "No JS code block found in the input. input:" << input.str()
              << std::endl;
  }

  std::string runnable_code_suffix =
      ""
      "let resultValue = compute();\n"
      "resultValue.toString()";

  v0::Value output_pb;
  output_pb.set_str(output + runnable_code_suffix);
  return output_pb;
}

absl::StatusOr<v0::Value> ReAct::ExtractMathQuestion(v0::Value input) {
  RE2 math_pattern("Math\\[(.*?)\\]", RE2::Quiet);
  std::string math_question;
  RE2::PartialMatch(input.str(), math_pattern, &math_question);

  v0::Value result;
  result.set_str(math_question);
  return result;
}

namespace {
// Extract everything before the first "Observation" line, discard the rest. If
// not found, return input string.

std::string BeforeFirstObservationOrOriginalText(const std::string& text) {
  size_t pos = text.find("Observation");
  if (pos != std::string::npos) {
    return text.substr(0, pos);
  }
  return text;
}

// Extract "Finish[...]" from llm output. If not found reutrn empty string.
std::string ExtractFinish(const std::string& text) {
  RE2 pattern("(Finish\\[.*?\\])");
  std::string match;
  // Search for the pattern in the text
  if (RE2::PartialMatch(text, pattern, &match)) {
    return match;
  }
  return "";
}

}  // namespace

absl::StatusOr<v0::Value> ReAct::ParseThoughtAction(v0::Value input) {
  std::string sanitized_input =
      BeforeFirstObservationOrOriginalText(input.str());
  RE2 thought_pattern("Thought:\\s*(?:\n)?(.*?)\n", RE2::Quiet);
  RE2 action_pattern("Action:\\s*(?:\n)?([^\\n]+)", RE2::Quiet);

  std::string thought;
  std::string action;
  RE2::PartialMatch(sanitized_input, thought_pattern, &thought);
  if (!RE2::PartialMatch(sanitized_input, action_pattern, &action)) {
    action = ExtractFinish(sanitized_input);
  }

  std::string thought_action =
      absl::StrFormat("Thought: %s\nAction: %s\n", thought, action);

  v0::Value result;
  result.set_str(thought_action);
  return result;
}

absl::StatusOr<v0::Value> ReAct::FormatObservation(v0::Value input) {
  v0::Value result;
  result.set_str(absl::StrFormat("Observation: %s\n", input.str()));
  return result;
}

absl::Status ReAct::SetCustomFunctions(
    intrinsics::CustomFunction::FunctionMap& fn_map) {
  fn_map["/react/format_observation"] = [](const v0::Value& arg) {
    return ReAct::FormatObservation(arg);
  };

  fn_map["/react/parse_thought_action"] = [](const v0::Value& arg) {
    return ReAct::ParseThoughtAction(arg);
  };

  fn_map["/react/extract_math_question"] = [](const v0::Value& arg) {
    return ReAct::ExtractMathQuestion(arg);
  };

  fn_map["/react/extract_python_code"] = [](const v0::Value& arg) {
    return ReAct::ExtractPythonCode(arg);
  };

  fn_map["/react/extract_js_code"] = [](const v0::Value& arg) {
    return ReAct::ExtractJsCode(arg);
  };

  return absl::OkStatus();
}
}  // namespace genc
