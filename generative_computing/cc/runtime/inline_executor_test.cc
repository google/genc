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

#include "generative_computing/cc/runtime/inline_executor.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <streambuf>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/authoring/constructor.h"
#include "generative_computing/cc/intrinsics/handler_sets.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/runner.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace {

class InlineExecutorTest : public ::testing::Test {
 protected:
  InlineExecutorTest() {}
  ~InlineExecutorTest() override {}

  // TODO(b/295260921): This needs to test a setup with multiple executors that
  // handle models for various URI prefixes, that calls are routed to an
  // appropriate child executor depending on the model URI, and that bogus URIs
  // result in an error.
};

TEST_F(InlineExecutorTest, TestModel) {
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      CreateInlineExecutor(intrinsics::CreateCompleteHandlerSet({}));
  EXPECT_TRUE(executor.ok());

  v0::Value fn_pb = CreateModelInference("test_model").value();
  absl::StatusOr<OwnedValueId> fn_val = executor.value()->CreateValue(fn_pb);
  EXPECT_TRUE(fn_val.ok());

  v0::Value arg_pb;
  arg_pb.set_str("Boo!");
  absl::StatusOr<OwnedValueId> arg_val = executor.value()->CreateValue(arg_pb);
  EXPECT_TRUE(arg_val.ok());

  absl::StatusOr<OwnedValueId> result_val =
      executor.value()->CreateCall(fn_val->ref(), arg_val->ref());
  EXPECT_TRUE(result_val.ok());

  v0::Value result;
  absl::Status status =
      executor.value()->Materialize(result_val->ref(), &result);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(result.str(),
            "This is an output from a test model in response to \"Boo!\".");
}

TEST_F(InlineExecutorTest, TestModelWithInferenceFn) {
  intrinsics::HandlerSetConfig config;
  config.model_inference_map["test_inference_fn"] = [](v0::Value arg) {
    v0::Value result;
    result.set_str(absl::StrCat("Testing inference fn with arg: ", arg.str()));
    return result;
  };
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      CreateInlineExecutor(intrinsics::CreateCompleteHandlerSet(config));
  EXPECT_TRUE(executor.ok());

  v0::Value fn_pb = CreateModelInference("test_inference_fn").value();
  absl::StatusOr<OwnedValueId> fn_val = executor.value()->CreateValue(fn_pb);
  EXPECT_TRUE(fn_val.ok());

  v0::Value arg_pb;
  arg_pb.set_str("Boo!");
  absl::StatusOr<OwnedValueId> arg_val = executor.value()->CreateValue(arg_pb);
  EXPECT_TRUE(arg_val.ok());

  absl::StatusOr<OwnedValueId> result_val =
      executor.value()->CreateCall(fn_val->ref(), arg_val->ref());
  EXPECT_TRUE(result_val.ok());

  v0::Value result;
  absl::Status status =
      executor.value()->Materialize(result_val->ref(), &result);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(result.str(), "Testing inference fn with arg: Boo!");
}

TEST_F(InlineExecutorTest, CustomFunctionInvokesUserDefinedFn) {
  intrinsics::HandlerSetConfig config;
  config.custom_function_map["append_foo"] = [](v0::Value arg) {
    v0::Value result;
    result.set_str(absl::StrCat(arg.str(), "foo"));
    return result;
  };
  std::shared_ptr<Executor> executor =
      CreateInlineExecutor(intrinsics::CreateCompleteHandlerSet(config))
          .value();

  v0::Value fn_pb = CreateCustomFunction("append_foo").value();
  Runner runner = Runner::Create(executor).value();

  v0::Value arg_pb;
  arg_pb.set_str("bar");
  v0::Value result = runner.Run(fn_pb, arg_pb).value();
  EXPECT_EQ(result.str(), "barfoo");
}

TEST_F(InlineExecutorTest, ProcessUnivariatePromptTemplate) {
  std::shared_ptr<Executor> executor =
      CreateInlineExecutor(intrinsics::CreateCompleteHandlerSet({})).value();

  v0::Value template_pb =
      CreatePromptTemplate(
          "Q: What should I pack for a trip to {location}? A: ")
          .value();

  Runner runner = Runner::Create(executor).value();

  v0::Value arg_pb;
  arg_pb.set_str("a grocery store");
  v0::Value result = runner.Run(template_pb, arg_pb).value();
  EXPECT_EQ(result.str(),
            "Q: What should I pack for a trip to a grocery store? A: ");
}

TEST_F(InlineExecutorTest, ProcessUnivariatePromptTemplateFailsWithEmptyStr) {
  std::shared_ptr<Executor> executor =
      CreateInlineExecutor(intrinsics::CreateCompleteHandlerSet({})).value();

  v0::Value template_pb =
      CreatePromptTemplate(
          "Q: What should I pack for a trip to {location}? A: ")
          .value();

  Runner runner = Runner::Create(executor).value();

  v0::Value arg_pb;
  absl::StatusOr<v0::Value> result = runner.Run(template_pb, arg_pb);
  EXPECT_FALSE(result.ok());
}

TEST_F(InlineExecutorTest, ProcessMultivariatePromptTemplate) {
  std::shared_ptr<Executor> executor =
      CreateInlineExecutor(intrinsics::CreateCompleteHandlerSet({})).value();

  v0::Value template_pb =
      CreatePromptTemplate(
          "Q: What should I {do} for a trip to {location}? Also find me the "
          "cheapest transportation to {location}. A: ")
          .value();

  Runner runner = Runner::Create(executor).value();

  v0::Value arg_pb;
  // Variables in template can functions like keyword argument, therefore order
  // doesn't matter.
  v0::NamedValue* location_arg_pb =
      arg_pb.mutable_struct_()->mutable_element()->Add();
  location_arg_pb->set_name("location");
  location_arg_pb->mutable_value()->set_str("Tokyo");

  v0::NamedValue* do_arg_pb =
      arg_pb.mutable_struct_()->mutable_element()->Add();
  do_arg_pb->set_name("do");
  do_arg_pb->mutable_value()->set_str("pack");

  v0::Value result = runner.Run(template_pb, arg_pb).value();
  EXPECT_EQ(result.str(),
            "Q: What should I pack for a trip to Tokyo? Also find me the "
            "cheapest transportation to Tokyo. A: ");
}

TEST_F(InlineExecutorTest, CreateStructAndSelection) {
  absl::StatusOr<std::shared_ptr<Executor>> executor = CreateInlineExecutor(
      intrinsics::CreateCompleteHandlerSet(intrinsics::HandlerSetConfig()));
  EXPECT_TRUE(executor.ok());

  v0::Value x, y;
  x.set_str("foo");
  y.set_int_32(10);

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
  good_struct->add_element()->mutable_value()->set_str("foo");
  good_struct->add_element()->mutable_value()->set_int_32(10);
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

TEST_F(InlineExecutorTest, LoggerLogsAndLeavesValueUnchanged) {
  std::shared_ptr<Executor> executor =
      CreateInlineExecutor(intrinsics::CreateCompleteHandlerSet({})).value();

  v0::Value logger_pb = CreateLogger().value();
  Runner runner = Runner::Create(executor).value();

  std::streambuf* cout_streambuf = std::cout.rdbuf();
  std::ostringstream captured_output;
  std::cout.rdbuf(captured_output.rdbuf());
  v0::Value arg_pb;
  arg_pb.set_str("Boo!");
  runner.Run(logger_pb, arg_pb).value();
  std::cout.rdbuf(cout_streambuf);

  EXPECT_EQ(captured_output.str(), "Boo!\n");
}

}  // namespace
}  // namespace generative_computing
