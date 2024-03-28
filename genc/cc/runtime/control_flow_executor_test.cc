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

#include "genc/cc/runtime/control_flow_executor.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"
#include "genc/cc/authoring/constructor.h"
#include "genc/cc/intrinsics/custom_function.h"
#include "genc/cc/intrinsics/handler_sets.h"
#include "genc/cc/intrinsics/model_inference.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/inline_executor.h"
#include "genc/cc/runtime/intrinsic_handler.h"
#include "genc/cc/runtime/runner.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

constexpr absl::string_view kTestIntrinsic =
    "nonexistent_intrinsic_only_for_testing_purposes";

class ControlFlowExecutorTest : public ::testing::Test {
 protected:
  ControlFlowExecutorTest() {}
  ~ControlFlowExecutorTest() override {}
};

absl::StatusOr<std::shared_ptr<Executor>> CreateTestControlFlowExecutor(
    intrinsics::ModelInference::InferenceMap* inference_map = nullptr,
    intrinsics::CustomFunction::FunctionMap* custom_fn_map = nullptr) {
  std::shared_ptr<IntrinsicHandlerSet> handler_set;

  intrinsics::HandlerSetConfig config;
  if (custom_fn_map != nullptr) {
    config.custom_function_map = *custom_fn_map;
  }
  if (inference_map != nullptr) {
    config.model_inference_map = *inference_map;
  }
  handler_set = intrinsics::CreateCompleteHandlerSet(config);

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

  Runner runner = Runner::Create(executor).value();

  // Repeat loop is parameterized with input Value, each for loop the content of
  // Value evolves to next state, in this case, `foo` is appended per step.
  v0::Value arg;
  arg.set_str("");
  v0::Value result = runner.Run(repeat_pb, arg).value();

  EXPECT_EQ(result.str(), "foofoofoo");
}

TEST_F(ControlFlowExecutorTest, TestChaining) {
  // 1. Define any custom function, e.g. a call to some models behind a server.
  // These are typically owned by you, and heavily customized.
  intrinsics::CustomFunction::FunctionMap fn_map;
  fn_map["fn_1"] = [](v0::Value arg) {
    v0::Value result;
    result.set_str(absl::StrFormat("fn_1(%s)", arg.str()));
    return result;
  };

  fn_map["fn_2"] = [](v0::Value arg) {
    v0::Value result;
    result.set_str(absl::StrFormat("fn_2(%s)", arg.str()));
    return result;
  };

  // Make the runtime aware of these custom functions.
  std::shared_ptr<Executor> executor =
      CreateTestControlFlowExecutor(/*inference_map=*/nullptr, &fn_map).value();

  // 2.Chaining of function calls.
  // Below are code you author & experiment with typically in python or c++,
  // whichever is your language choice. The performance is the same.
  std::vector<v0::Value> fn_chain =
      std::vector<v0::Value>({CreateCustomFunction("fn_1").value(),
                              CreateCustomFunction("fn_2").value()});
  // Parameterize your chain.
  v0::Value parameterized_chain = CreateSerialChain(fn_chain).value();

  // 3. Run it.
  // These runtimes can be loaded behind server, mobile, and runs arbitrary
  // computation.
  Runner runner = Runner::Create(executor).value();

  v0::Value arg;
  arg.set_str("test_input");
  v0::Value result = runner.Run(parameterized_chain, arg).value();

  EXPECT_EQ(result.str(), "fn_2(fn_1(test_input))");
}

TEST_F(ControlFlowExecutorTest, ConditionalSelectTheRightBranchForExecution) {
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

  std::shared_ptr<Executor> executor =
      CreateTestControlFlowExecutor(&inference_map).value();
  Runner runner = Runner::Create(executor).value();

  // Create the operators
  v0::Value fn_detector = CreateRegexPartialMatch("append_foo_fn").value();
  v0::Value append_foo_fn = CreateModelInference("append_foo").value();
  v0::Value append_bar_fn = CreateModelInference("append_bar").value();

  v0::Value comp_pb =
      CreateLambdaForConditional(fn_detector, append_foo_fn, append_bar_fn)
          .value();

  // Run it.
  v0::Value arg;
  arg.set_str("call append_bar_fn:");
  v0::Value result = runner.Run(comp_pb, arg).value();
  EXPECT_EQ(result.str(), "call append_bar_fn:bar");

  arg.set_str("call append_foo_fn:");
  result = runner.Run(comp_pb, arg).value();
  EXPECT_EQ(result.str(), "call append_foo_fn:foo");
}

TEST_F(ControlFlowExecutorTest, WhileLoopExecutionTest) {
  // Create a test condition_fn that pumps the while loop.
  v0::Value test_condition_fn =
      CreateSerialChain({CreateRegexPartialMatch("Action: Finish").value(),
                         CreateLogicalNot().value()})
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

  Runner runner = Runner::Create(executor).value();
  v0::Value arg;
  arg.set_str("");
  v0::Value result = runner.Run(while_pb, arg).value();
  EXPECT_EQ(result.str(), "12Action: Finish");
}

TEST_F(ControlFlowExecutorTest, RepeatedConditionalChainCanToLoopAndBreak) {
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
      CreateRepeatedConditionalChain(
          100,
          std::vector<v0::Value>{append_foo_fn, if_finish_then_break_fn,
                                 append_bar_fn, count_to_3_append_finish_fn})
          .value();

  // Run it.
  Runner runner = Runner::Create(executor).value();
  v0::Value arg;
  arg.set_str("[START]");
  v0::Value result = runner.Run(comp_pb, arg).value();
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
  Runner runner = Runner::Create(executor).value();
  v0::Value arg;
  arg.set_str("[START]");
  v0::Value result = runner.Run(comp_pb, arg).value();
  EXPECT_EQ(result.str(), "[START]foobar");
}

TEST_F(ControlFlowExecutorTest, CreateStructAndSelection) {
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      CreateTestControlFlowExecutor();
  EXPECT_TRUE(executor.ok());

  v0::Value x, y;
  x.set_str("foo");
  y.set_str("bar");

  absl::StatusOr<OwnedValueId> x_val = executor.value()->CreateValue(x);
  EXPECT_TRUE(x_val.ok());

  absl::StatusOr<OwnedValueId> y_val = executor.value()->CreateValue(y);
  EXPECT_TRUE(y_val.ok());

  std::vector<ValueId> elements;
  elements.push_back(x_val.value().ref());
  elements.push_back(y_val.value().ref());
  absl::StatusOr<OwnedValueId> z_val = executor.value()->CreateStruct(elements);
  EXPECT_TRUE(z_val.ok());

  v0::Value result;
  EXPECT_TRUE(executor.value()->Materialize(z_val.value().ref(), &result).ok());

  v0::Value expected_result;
  auto good_struct = expected_result.mutable_struct_();
  good_struct->add_element()->set_str("foo");
  good_struct->add_element()->set_str("bar");
  EXPECT_EQ(result.DebugString(), expected_result.DebugString());

  absl::StatusOr<OwnedValueId> a_val =
      executor.value()->CreateSelection(z_val.value().ref(), 0);
  EXPECT_TRUE(a_val.ok());
  v0::Value a_pb;
  EXPECT_TRUE(executor.value()->Materialize(a_val.value().ref(), &a_pb).ok());
  EXPECT_EQ(a_pb.DebugString(), x.DebugString());

  absl::StatusOr<OwnedValueId> b_val =
      executor.value()->CreateSelection(z_val.value().ref(), 1);
  EXPECT_TRUE(b_val.ok());
  v0::Value b_pb;
  EXPECT_TRUE(executor.value()->Materialize(b_val.value().ref(), &b_pb).ok());
  EXPECT_EQ(b_pb.DebugString(), y.DebugString());
}

TEST_F(ControlFlowExecutorTest, CreateStructInIntrinsicHandler) {
  class TestIntrinsic : public ControlFlowIntrinsicHandlerBase {
   public:
    TestIntrinsic() : ControlFlowIntrinsicHandlerBase(kTestIntrinsic) {}
    virtual ~TestIntrinsic() {}

    absl::Status CheckWellFormed(
        const v0::Intrinsic& intrinsic_pb) const final {
      return absl::OkStatus();
    }

    // Constructs a struct that consists of the dynamic argument and the static
    // argument as the two elements.
    absl::StatusOr<ValueRef> ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                         std::optional<ValueRef> arg,
                                         Context* context) const final {
      if (!arg.has_value()) {
        return absl::InvalidArgumentError("Missing argument.");
      }
      std::shared_ptr<Value> x = arg.value();
      std::shared_ptr<Value> y =
          GENC_TRY(context->CreateValue(intrinsic_pb.static_parameter()));
      std::vector<std::shared_ptr<Value>> elements;
      elements.push_back(x);
      elements.push_back(y);
      return context->CreateStruct(absl::MakeSpan(elements));
    }
  };

  std::shared_ptr<IntrinsicHandlerSet> handler_set =
      intrinsics::CreateCompleteHandlerSet(intrinsics::HandlerSetConfig());
  handler_set->AddHandler(new TestIntrinsic());
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      CreateControlFlowExecutor(handler_set,
                                CreateInlineExecutor(handler_set).value());
  EXPECT_TRUE(executor.ok());

  v0::Value x, y;
  x.set_str("foo");
  auto intrinsic = y.mutable_intrinsic();
  intrinsic->set_uri(std::string(kTestIntrinsic));
  intrinsic->mutable_static_parameter()->set_str("bar");

  absl::StatusOr<OwnedValueId> x_val = executor.value()->CreateValue(x);
  EXPECT_TRUE(x_val.ok());

  absl::StatusOr<OwnedValueId> y_val = executor.value()->CreateValue(y);
  EXPECT_TRUE(y_val.ok());

  absl::StatusOr<OwnedValueId> z_val =
      executor.value()->CreateCall(y_val.value().ref(), x_val.value().ref());
  EXPECT_TRUE(z_val.ok());

  v0::Value result;
  EXPECT_TRUE(executor.value()->Materialize(z_val.value().ref(), &result).ok());

  v0::Value expected_result;
  auto good_struct = expected_result.mutable_struct_();
  good_struct->add_element()->set_str("foo");
  good_struct->add_element()->set_str("bar");
  EXPECT_EQ(result.DebugString(), expected_result.DebugString());
}

TEST_F(ControlFlowExecutorTest, ParallelMapOnSuccessAppliesFnToAllArguments) {
  intrinsics::CustomFunction::FunctionMap fn_map;
  fn_map["apply_fn"] = [](v0::Value arg) {
    v0::Value result;
    result.set_str(absl::StrFormat("fn(%s)", arg.str()));
    return result;
  };
  std::shared_ptr<Executor> executor =
      CreateTestControlFlowExecutor(/*inference_map=*/nullptr, &fn_map).value();

  v0::Value fn_pb = CreateCustomFunction("apply_fn").value();
  v0::Value comp_pb = CreateParallelMap(fn_pb).value();

  Runner runner = Runner::Create(executor).value();

  v0::Value x;
  x.mutable_struct_()->add_element()->set_str("foo");
  x.mutable_struct_()->add_element()->set_str("bar");
  v0::Value result = runner.Run(comp_pb, x).value();

  v0::Value expected_result;
  auto good_struct = expected_result.mutable_struct_();
  good_struct->add_element()->set_str("fn(foo)");
  good_struct->add_element()->set_str("fn(bar)");
  EXPECT_EQ(result.DebugString(), expected_result.DebugString());
}

TEST_F(ControlFlowExecutorTest, CreateSelectionInIntrinsicHandler) {
  class TestIntrinsic : public ControlFlowIntrinsicHandlerBase {
   public:
    TestIntrinsic() : ControlFlowIntrinsicHandlerBase(kTestIntrinsic) {}
    virtual ~TestIntrinsic() {}

    absl::Status CheckWellFormed(
        const v0::Intrinsic& intrinsic_pb) const final {
      return absl::OkStatus();
    }

    // Selects tjhe first element of a struct.
    absl::StatusOr<ValueRef> ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                         std::optional<ValueRef> arg,
                                         Context* context) const final {
      if (!arg.has_value()) {
        return absl::InvalidArgumentError("Missing argument.");
      }
      return context->CreateSelection(arg.value(), 0);
    }
  };

  std::shared_ptr<IntrinsicHandlerSet> handler_set =
      intrinsics::CreateCompleteHandlerSet(intrinsics::HandlerSetConfig());
  handler_set->AddHandler(new TestIntrinsic());
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      CreateControlFlowExecutor(handler_set,
                                CreateInlineExecutor(handler_set).value());
  EXPECT_TRUE(executor.ok());

  v0::Value x, y;
  x.mutable_struct_()->add_element()->set_str("foo");
  x.mutable_struct_()->add_element()->set_str("bar");
  y.mutable_intrinsic()->set_uri(std::string(kTestIntrinsic));

  absl::StatusOr<OwnedValueId> x_val = executor.value()->CreateValue(x);
  EXPECT_TRUE(x_val.ok());

  absl::StatusOr<OwnedValueId> y_val = executor.value()->CreateValue(y);
  EXPECT_TRUE(y_val.ok());

  absl::StatusOr<OwnedValueId> z_val =
      executor.value()->CreateCall(y_val.value().ref(), x_val.value().ref());
  EXPECT_TRUE(z_val.ok());

  v0::Value result;
  EXPECT_TRUE(executor.value()->Materialize(z_val.value().ref(), &result).ok());
  EXPECT_EQ(result.DebugString(), x.struct_().element(0).DebugString());
}

TEST_F(ControlFlowExecutorTest, CanProcessStruct) {
  std::shared_ptr<Executor> executor = CreateTestControlFlowExecutor().value();

  v0::Value template_pb =
      CreatePromptTemplate(
          "Q: What should I {do} for a trip to {location}? Also find me the "
          "cheapest transportation to {location}. A: ")
          .value();

  Runner runner = Runner::Create(executor).value();

  v0::Value arg_pb;
  // Variables in template can functions like keyword argument, therefore order
  // doesn't matter.
  v0::Value* location_arg_pb =
      arg_pb.mutable_struct_()->mutable_element()->Add();
  location_arg_pb->set_label("location");
  location_arg_pb->set_str("Tokyo");

  v0::Value* do_arg_pb = arg_pb.mutable_struct_()->mutable_element()->Add();
  do_arg_pb->set_label("do");
  do_arg_pb->set_str("pack");

  v0::Value result = runner.Run(template_pb, arg_pb).value();
  EXPECT_EQ(result.str(),
            "Q: What should I pack for a trip to Tokyo? Also find me the "
            "cheapest transportation to Tokyo. A: ");
}

TEST_F(ControlFlowExecutorTest, CanModelComplexChainAsBlock) {
  intrinsics::CustomFunction::FunctionMap fn_map;
  fn_map["add_1"] = [](v0::Value arg) {
    v0::Value result;
    result.set_int_32(arg.int_32() + 1);
    return result;
  };

  fn_map["add_2"] = [](v0::Value arg) {
    v0::Value result;
    result.set_int_32(arg.int_32() + 2);
    return result;
  };

  fn_map["add_3"] = [](v0::Value arg) {
    v0::Value result;
    result.set_int_32(arg.int_32() + 3);
    return result;
  };

  fn_map["sum"] = [](v0::Value arg) {
    v0::Value result;
    int sum = 0;
    for (const auto& val : arg.struct_().element()) {
      sum += val.int_32();
    }
    result.set_int_32(sum);
    return result;
  };

  std::shared_ptr<Executor> executor =
      CreateTestControlFlowExecutor(/*inference_map=*/nullptr, &fn_map).value();
  Runner runner = Runner::Create(executor).value();

  v0::Value add_1 = CreateCustomFunction("add_1").value();
  v0::Value add_2 = CreateCustomFunction("add_2").value();

  v0::Value result_pb;
  v0::Block* block = result_pb.mutable_block();

  // 1. define the inputs
  v0::Block::Local* input_1 = block->add_local();
  input_1->set_name("input_1");
  *input_1->mutable_value() =
      CreateSelection(CreateReference("arg").value(), 0).value();

  v0::Block::Local* input_2 = block->add_local();
  input_2->set_name("input_2");
  *input_2->mutable_value() =
      CreateSelection(CreateReference("arg").value(), 1).value();

  // 2. input_1 --> add_1 --> add_2 --> chain_1_output (=4)
  v0::Value chain_1 = CreateSerialChain({add_1, add_2}).value();
  v0::Block::Local* chain_1_output = block->add_local();
  chain_1_output->set_name("chain_1_output");
  *chain_1_output->mutable_value() =
      CreateCall(chain_1, CreateReference("input_1").value()).value();

  // 2. input_2 --> add_3 --> chain_2_output (=5)
  v0::Value chain_2 = CreateCustomFunction("add_3").value();
  v0::Block::Local* chain_2_output = block->add_local();
  chain_2_output->set_name("chain_2_output");
  *chain_2_output->mutable_value() =
      CreateCall(chain_2, CreateReference("input_2").value()).value();

  // 3. collect input_1 (1), chain_1_output (4), chain_2_output (5) as a Struct.
  v0::Value combined_output;
  *combined_output.mutable_struct_()->add_element() =
      CreateReference("input_1").value();
  *combined_output.mutable_struct_()->add_element() =
      CreateReference("chain_1_output").value();
  *combined_output.mutable_struct_()->add_element() =
      CreateReference("chain_2_output").value();

  // 4. Sum it up
  *block->mutable_result() =
      CreateCall(CreateCustomFunction("sum").value(), combined_output).value();

  // 5. Parameterize the chain
  v0::Value computation = CreateLambda("arg", result_pb).value();

  // Finally run the computation
  v0::Value input;
  input.mutable_struct_()->add_element()->set_int_32(1);
  input.mutable_struct_()->add_element()->set_int_32(2);
  v0::Value result = runner.Run(computation, input).value();
  EXPECT_EQ(result.int_32(), 10);
}

}  // namespace genc
