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
#include "generative_computing/proto/v0/computation.pb.h"

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
    intrinsics::HandlerSetConfig config;
    config.model_inference_map = *inference_map;
    handler_set = intrinsics::CreateCompleteHandlerSet(config);
  }

  return CreateControlFlowExecutor(handler_set,
                                   CreateInlineExecutor(handler_set).value());
}

TEST_F(ControlFlowExecutorTest, ReturnsExecutorOnCreation) {
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      CreateTestControlFlowExecutor();

  EXPECT_TRUE(executor.ok());
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

  v0::Value body_fn = CreateModelInference("append_foo").value();

  std::shared_ptr<Executor> executor =
      CreateTestControlFlowExecutor(&inference_map).value();

  v0::Value repeat_pb = CreateRepeat(3, body_fn).value();

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

TEST_F(ControlFlowExecutorTest, WhileLoopExecutionTest) {
  // Create a test condition_fn that pumps the while loop.
  v0::Value test_condition_fn =
      CreateChain({CreateLogicalNot().value(),
                   CreateRegexPartialMatch("Action: Finish").value()})
          .value();

  // Create a test body_fn.
  intrinsics::ModelInference::InferenceMap inference_map;
  v0::Value test_body_fn = CreateModelInference("test_body_fn").value();
  int current_val = 1;
  inference_map["test_body_fn"] = [&current_val](const v0::Value& arg) {
    v0::Value result;
    if (current_val == 3) {
      result.set_str(absl::StrCat(arg.str(), "Action: Finish"));
    } else {
      result.set_str(absl::StrCat(arg.str(), current_val));
    }
    current_val++;
    return result;
  };

  std::shared_ptr<Executor> executor =
      CreateTestControlFlowExecutor(&inference_map).value();
  v0::Value while_pb = CreateWhile(test_condition_fn, test_body_fn).value();

  Runner runner = Runner::Create(while_pb, executor).value();
  v0::Value arg;
  arg.set_str("");
  v0::Value result = runner.Run(arg).value();
  EXPECT_EQ(result.str(), "12Action: Finish");
}

TEST_F(ControlFlowExecutorTest, LoopChainComboAbleToLoopAndBreak) {
  // Step up executor and test custom fns.
  intrinsics::ModelInference::InferenceMap inference_map;
  v0::Value test_body_fn = CreateModelInference("test_body_fn").value();
  int count = 0;
  inference_map["append_finish_when_counts_reaches_3"] =
      [&count](const v0::Value& arg) {
        v0::Value result;
        count++;
        if (count == 3) {
          result.set_str(absl::StrCat(arg.str(), "[FINISH]"));
          return result;
        }
        return arg;
      };

  inference_map["append_foo"] = [](const v0::Value& arg) {
    v0::Value result;
    result.set_str(absl::StrCat(arg.str(), "foo"));
    return result;
  };

  inference_map["append_bar"] = [](const v0::Value& arg) {
    v0::Value result;
    result.set_str(absl::StrCat(arg.str(), "bar"));
    return result;
  };
  std::shared_ptr<Executor> executor =
      CreateTestControlFlowExecutor(&inference_map).value();

  // Create a for loop computation with break condition.
  v0::Value append_foo_fn = CreateModelInference("append_foo").value();
  v0::Value append_bar_fn = CreateModelInference("append_bar").value();
  v0::Value if_finish_then_break_fn = CreateRegexPartialMatch("FINISH").value();
  v0::Value count_to_3_append_finish_fn =
      CreateModelInference("append_finish_when_counts_reaches_3").value();
  v0::Value comp_pb =
      CreateLoopChainCombo(
          100,
          std::vector<v0::Value>{append_foo_fn, if_finish_then_break_fn,
                                 append_bar_fn, count_to_3_append_finish_fn})
          .value();

  // Run it.
  Runner runner = Runner::Create(comp_pb, executor).value();
  v0::Value arg;
  arg.set_str("[START]");
  v0::Value result = runner.Run(arg).value();
  EXPECT_EQ(result.str(), "[START]foobarfoobarfoobar[FINISH]foo");
}

TEST_F(ControlFlowExecutorTest, BreakableChainCanChainAndBreak) {
  intrinsics::ModelInference::InferenceMap inference_map;
  inference_map["append_foo"] = [](const v0::Value& arg) {
    v0::Value result;
    result.set_str(absl::StrCat(arg.str(), "foo"));
    return result;
  };

  inference_map["append_bar"] = [](const v0::Value& arg) {
    v0::Value result;
    result.set_str(absl::StrCat(arg.str(), "bar"));
    return result;
  };

  inference_map["append_baz"] = [](const v0::Value& arg) {
    v0::Value result;
    result.set_str(absl::StrCat(arg.str(), "baz"));
    return result;
  };
  std::shared_ptr<Executor> executor =
      CreateTestControlFlowExecutor(&inference_map).value();

  // Create a custom chain with break condition.
  v0::Value append_foo_fn = CreateModelInference("append_foo").value();
  v0::Value append_bar_fn = CreateModelInference("append_bar").value();
  v0::Value break_with_bar_fn = CreateRegexPartialMatch("bar").value();
  v0::Value append_baz_fn = CreateModelInference("append_baz").value();
  v0::Value comp_pb =
      CreateBreakableChain(std::vector<v0::Value>{append_foo_fn, append_bar_fn,
                                                  break_with_bar_fn,
                                                  append_baz_fn})
          .value();

  // Run it.
  Runner runner = Runner::Create(comp_pb, executor).value();
  v0::Value arg;
  arg.set_str("[START]");
  v0::Value result = runner.Run(arg).value();
  EXPECT_EQ(result.str(), "[START]foobar");
}
}  // namespace generative_computing