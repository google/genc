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

#include <string>
#include <vector>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "genc/cc/authoring/constructor.h"
#include "genc/cc/runtime/executor_stacks.h"
#include "genc/cc/runtime/runner.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace {

absl::StatusOr<v0::Value> MakeTestArg() {
  v0::Value foo_val, bar_val;
  foo_val.set_str("XXX");
  bar_val.set_str("YYY");
  std::vector<v0::Value> elements;
  elements.push_back(GENC_TRY(CreateNamedValue("foo", foo_val)));
  elements.push_back(GENC_TRY(CreateNamedValue("bar", bar_val)));
  return CreateStruct(elements);
}

absl::StatusOr<v0::Value> RunFuncOnArg(v0::Value func, v0::Value arg) {
  return GENC_TRY(Runner::Create(
      func, GENC_TRY(CreateDefaultLocalExecutor()))).Run(arg);
}

TEST(MultivariatePromptTemplateTest, Simple) {
  absl::StatusOr<v0::Value> func = CreatePromptTemplate(
      "A template in which a foo is {foo} and a bar is {bar}.");
  ASSERT_OK(func);

  absl::StatusOr<v0::Value> arg = MakeTestArg();
  ASSERT_OK(arg);
  absl::StatusOr<v0::Value> result = RunFuncOnArg(func.value(), arg.value());
  EXPECT_OK(result);
  EXPECT_EQ(result.value().str(),
            "A template in which a foo is XXX and a bar is YYY.");
}

absl::StatusOr<v0::Value> WrapInLambda(v0::Value func) {
  const std::string arg_name = "arg";
  v0::Value arg_ref = GENC_TRY(CreateReference(arg_name));
  std::vector<v0::Value> elements;
  elements.push_back(GENC_TRY(CreateNamedValue(
      "foo", GENC_TRY(CreateSelection(arg_ref, 0)))));
  elements.push_back(GENC_TRY(CreateNamedValue(
      "bar", GENC_TRY(CreateSelection(arg_ref, 1)))));
  return CreateLambda(arg_name, GENC_TRY(CreateCall(
      func, GENC_TRY(CreateStruct(elements)))));
}

TEST(MultivariatePromptTemplateTest, WrappedInLambda) {
  absl::StatusOr<v0::Value> func = CreatePromptTemplate(
      "A template in which a foo is {foo} and a bar is {bar}.");
  ASSERT_OK(func);
  func = WrapInLambda(func.value());
  ASSERT_OK(func);

  absl::StatusOr<v0::Value> arg = MakeTestArg();
  ASSERT_OK(arg);
  absl::StatusOr<v0::Value> result = RunFuncOnArg(func.value(), arg.value());
  EXPECT_OK(result);
  EXPECT_EQ(result.value().str(),
            "A template in which a foo is XXX and a bar is YYY.");
}

}  // namespace
}  // namespace genc
