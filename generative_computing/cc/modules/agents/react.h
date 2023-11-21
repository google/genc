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

#ifndef GENERATIVE_COMPUTING_CC_MODULES_AGENTS_REACT_H_
#define GENERATIVE_COMPUTING_CC_MODULES_AGENTS_REACT_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/intrinsics/custom_function.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

// Example prompt templates that works with utility methods in this class. They
// are examples to illustrate how things work together, in production, please
// perform prompt engineering and customize utility functions to improve agent
// quality.
constexpr char kMathPromptTemplateJs[] =
    "Translate the math problem below into a Javascript function named "
    "`compute` that can be executed to provide the answer to the "
    "problem\nMath Problem: {question}\nSolution:";

constexpr char kMathPromptTemplatePy[] =
    "Translate the math problem below into a Python function named "
    "`compute` that can be executed to provide the answer to the "
    "problem\nMath Problem: {question}\nSolution:";

constexpr char kMathReActInstructionTemplate[] =
    "Solve a question answering task with interleaving Thought, Action, "
    "Observation steps. Thought can reason about the current situation, and "
    "Action can be only two types:\n(1) Math[query], Useful for when you "
    "need to solve math problems. Input should be a math problem to be "
    "solved.\n(2) Finish[answer], which returns the answer and "
    "finishes the task. Please don't rush to answer, base the answer on past "
    "actions and observations.\nQuestion: {question}\n";

// Common utils for building ReAct agents. These functions are closely tied to
// use cases and prompts. We provide them as an example and quick building
// blocks. GenC is meant to be extensible. Please do not consider customize them
// in your own application.
class ReAct final {
 public:
  ~ReAct() = default;

  // Given LLM output contains python code, extract it.
  static absl::StatusOr<v0::Value> ExtractPythonCode(v0::Value input);

  // Given LLM output contains JS code, extract it.
  static absl::StatusOr<v0::Value> ExtractJsCode(v0::Value input);

  // Give a template "Math[...]" extract ..., which is usually a math question.
  static absl::StatusOr<v0::Value> ExtractMathQuestion(v0::Value input);

  // Typical ReAct Prompt structures llm output in Thought,Action pairs. This
  // function parses the LLM output, extact the thought & action pair from it
  // and discard the rest.
  static absl::StatusOr<v0::Value> ParseThoughtAction(v0::Value input);

  // Prefix LLM output with "Observation:".
  static absl::StatusOr<v0::Value> FormatObservation(v0::Value input);

  // Make ReAct functions visible to the runtime.
  static absl::Status SetCustomFunctions(
      intrinsics::CustomFunction::FunctionMap& fn_map);

  // Not copyable or movable.
  ReAct(const ReAct&) = delete;
  ReAct& operator=(const ReAct&) = delete;

 private:
  // Do not hold states in this class.
  ReAct() = default;
};
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_MODULES_AGENTS_REACT_H_
