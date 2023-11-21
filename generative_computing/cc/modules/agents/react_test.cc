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

#include "generative_computing/cc/modules/agents/react.h"

#include <string>

#include "googletest/include/gtest/gtest.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {

TEST(ParseThoughtActionTest, OnSuccessReturnsFormatedThoughtActionPair) {
  std::string test_str =
      "Thought: I need to find the square root of 4.\nAction: "
      "Math[sqrt(4)]\nObservation: 2\n";

  v0::Value input_pb;
  input_pb.set_str(test_str);

  EXPECT_EQ(
      ReAct::ParseThoughtAction(input_pb).value().str(),
      "Thought: I need to find the square root of 4.\nAction: Math[sqrt(4)]\n");
}

TEST(FormatObservationTest, OnSuccessReturnsFormatedObservation) {
  std::string test_str = "test_obs";

  v0::Value input_pb;
  input_pb.set_str(test_str);

  EXPECT_EQ(ReAct::FormatObservation(input_pb).value().str(),
            "Observation: test_obs\n");
}

TEST(ExtractMathQuestionTest, OnSuccessReturnsMathQuestion) {
  std::string test_str =
      "Some text....\n"
      "```python"
      "def compute():\n"
      " return 1\n"
      "```\n"
      "Some more text....\n";

  std::string expected_str =
      "def compute():\n"
      " return 1\n"
      "result = compute()\n"
      "print(result)\n";
  v0::Value input_pb;
  input_pb.set_str(test_str);
  EXPECT_EQ(ReAct::ExtractPythonCode(input_pb).value().str(), expected_str);
}
}  // namespace
}  // namespace generative_computing
