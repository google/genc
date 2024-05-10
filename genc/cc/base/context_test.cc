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

#include "genc/cc/base/context.h"

#include <memory>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace {

class TestContext : public Context {
 public:
  absl::StatusOr<v0::Value> Call(const v0::Value& portable_ir,
                                 const std::vector<v0::Value>& args) override {
    return v0::Value::default_instance();
  }

  ~TestContext() override {}
};

TEST(ContextStackTest, CanSetAndRetrieveContext) {
  auto base_context = std::make_shared<TestContext>();
  ContextStack stack;
  stack.SetDefaultContext(base_context);

  ASSERT_EQ(base_context, stack.CurrentContext());
}

TEST(ContextStackTest, CanAppendContext) {
  auto base_context = std::make_shared<TestContext>();
  auto second_context = std::make_shared<TestContext>();
  ContextStack stack(base_context);
  stack.AppendNestedContext(second_context);
  ASSERT_EQ(second_context, stack.CurrentContext());
}

TEST(ContextStackTest, CanRemoveNestedContext) {
  auto base_context = std::make_shared<TestContext>();
  auto second_context = std::make_shared<TestContext>();
  ContextStack stack(base_context);
  stack.AppendNestedContext(second_context);

  auto status = stack.RemoveNestedContext(second_context);
  ASSERT_TRUE(status.ok());
  ASSERT_EQ(base_context, stack.CurrentContext());
}

TEST(ContextStackTest, CannotRemoveContextNotAtTop) {
  auto base_context = std::make_shared<TestContext>();
  auto second_context = std::make_shared<TestContext>();
  ContextStack stack(base_context);

  stack.AppendNestedContext(second_context);
  auto status = stack.RemoveNestedContext(base_context);
  ASSERT_EQ(status.message(),
            "Can only remove the top context from the stack.");
}

// Tests the error case of removing from an empty stack
TEST(ContextStackTest, CannotRemoveFromEmpty) {
  auto base_context = std::make_shared<TestContext>();
  ContextStack stack;
  auto status = stack.RemoveNestedContext(base_context);
  ASSERT_EQ(status.message(), "No nested context on the stack.");
}

TEST(ContextStackProviderTest, ProvidesAccessToSingletonStack) {
  auto base_context = std::make_shared<TestContext>();
  GetContextStack()->SetDefaultContext(base_context);

  auto second_context = std::make_shared<TestContext>();
  GetContextStack()->AppendNestedContext(second_context);
  ASSERT_EQ(second_context, GetContextStack()->CurrentContext());
}

}  // namespace
}  // namespace genc
