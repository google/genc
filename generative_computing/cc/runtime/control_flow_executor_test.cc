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

#include "generative_computing/cc/runtime/control_flow_executor.h"

#include <memory>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "generative_computing/cc/authoring/constructor.h"
#include "generative_computing/cc/intrinsics/handler_sets.h"
#include "generative_computing/cc/intrinsics/model_inference.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/inline_executor.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/runner.h"

namespace generative_computing {

class ControlFlowExecutorTest : public ::testing::Test {
 protected:
  ControlFlowExecutorTest() {}
  ~ControlFlowExecutorTest() override {}
};

absl::StatusOr<std::shared_ptr<Executor>> CreateTestControlFlowExecutor(
    intrinsics::ModelInference::InferenceMap* inference_map = nullptr) {
  std::shared_ptr<IntrinsicHandlerSet> handler_set;
  if (inference_map == nullptr) {
    handler_set = intrinsics::CreateCompleteHandlerSet({});
  } else {
    handler_set = intrinsics::CreateCompleteHandlerSet(*inference_map);
  }

  return CreateControlFlowExecutor(handler_set,
                                   CreateInlineExecutor(handler_set).value());
}

TEST_F(ControlFlowExecutorTest, ReturnsExecutorOnCreation) {
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      CreateTestControlFlowExecutor();

  EXPECT_TRUE(executor.ok());

  // TODO(b/295041821): Pull in the tests.
}

TEST_F(ControlFlowExecutorTest, RepeatOnSuccessRunsComputationNTimes) {
  // Create a custom test inference function.
  // Custom Functions are predefined lambda, multimodal value-in-value-out.
  intrinsics::ModelInference::InferenceMap inference_map;
  inference_map["append_foo"] = [](const v0::Value& arg) {
    v0::Value result;
    result.set_str(absl::StrCat(arg.str(), "foo"));
    return result;
  };

  std::shared_ptr<Executor> executor =
      CreateTestControlFlowExecutor(&inference_map).value();

  v0::Value repeat_pb = CreateRepeat(3, "append_foo").value();

  Runner runner = Runner::Create(repeat_pb, executor).value();

  // Repeat loop is parameterized with input Value, each for loop the content of
  // Value evolves to next state, in this case, `foo` is appended per step.
  v0::Value arg;
  arg.set_str("");
  v0::Value result = runner.Run(arg).value();

  EXPECT_EQ(result.str(), "foofoofoo");
}

TEST_F(ControlFlowExecutorTest, TestChaining) {
  // 1. Define any custom function, e.g. a call to some models behind a server.
  // These are typically owned by you, and heavily customized.
  intrinsics::ModelInference::InferenceMap inference_map;
  inference_map["fn_1"] = [](v0::Value arg) {
    v0::Value result;
    result.set_str(absl::StrFormat("fn_1(%s)", arg.str()));
    return result;
  };

  inference_map["fn_2"] = [](v0::Value arg) {
    v0::Value result;
    result.set_str(absl::StrFormat("fn_2(%s)", arg.str()));
    return result;
  };

  // Make the runtime aware of these custom functions.
  std::shared_ptr<Executor> executor =
      CreateTestControlFlowExecutor(&inference_map).value();

  // 2.Chaining of function calls.
  // Below are code you author & experiment with typically in python or c++,
  // whichever is your language choice. The performance is the same.
  std::vector<v0::Value> fn_chain =
      std::vector<v0::Value>({CreateModelInference("fn_1").value(),
                              CreateModelInference("fn_2").value()});
  // Parameterize your chain.
  v0::Value parameterized_chain = CreateChain(fn_chain).value();

  // 3. Run it.
  // These runtimes can be loaded behind server, mobile, and runs arbitrary
  // computation.
  Runner runner = Runner::Create(parameterized_chain, executor).value();

  v0::Value arg;
  arg.set_str("test_input");
  v0::Value result = runner.Run(arg).value();

  EXPECT_EQ(result.str(), "fn_1(fn_2(test_input))");
}

}  // namespace generative_computing
