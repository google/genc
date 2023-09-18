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

#include "generative_computing/cc/runtime/executor.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"
#include "generative_computing/proto/v0/executor.pb.h"

namespace generative_computing {
namespace {

class ExecutorValue {
 public:
  ExecutorValue(absl::string_view x) { x_ = x; }
  absl::string_view value() const { return x_; }

 private:
  std::string x_;
};

class TestExecutor : public ExecutorBase<ExecutorValue> {
 public:
  explicit TestExecutor() {}

  ~TestExecutor() override { ClearTracked(); }

  absl::string_view ExecutorName() final {
    static constexpr absl::string_view kExecutorName = "TestExecutor";
    return kExecutorName;
  }

  absl::StatusOr<ExecutorValue> CreateExecutorValue(const v0::Value& v) final {
    return ExecutorValue(v.str());
  }

  absl::StatusOr<ExecutorValue> CreateCall(
      ExecutorValue function, std::optional<ExecutorValue> argument) final {
    return ExecutorValue(
        absl::StrCat(function.value(), "(", argument.value().value(), ")"));
  }

  absl::StatusOr<ExecutorValue> CreateStruct(
      std::vector<ExecutorValue> members) final {
    std::vector<absl::string_view> member_strings;
    for (const auto& member : members) {
      member_strings.push_back(member.value());
    }
    return ExecutorValue(
        absl::StrCat("(", absl::StrJoin(member_strings, ","), ")"));
  }

  absl::StatusOr<ExecutorValue> CreateSelection(ExecutorValue value,
                                                const uint32_t index) final {
    return ExecutorValue(absl::StrCat(value.value(), "[", index, "]"));
  }

  absl::Status Materialize(ExecutorValue value, v0::Value* val) final {
    val->set_str(std::string(value.value()));
    return absl::OkStatus();
  }
};

}  // namespace

class ExecutorTest : public ::testing::Test {
 protected:
  ExecutorTest() {}
  ~ExecutorTest() override {}
};

TEST_F(ExecutorTest, Simple) {
  const std::shared_ptr<Executor> executor = std::make_shared<TestExecutor>();
  v0::Value v1_pb;
  v1_pb.set_str("some_arg");
  auto v1 = executor->CreateValue(v1_pb);
  EXPECT_TRUE(v1.ok());
  v0::Value v2_pb;
  v2_pb.set_str("some_func");
  auto v2 = executor->CreateValue(v2_pb);
  EXPECT_TRUE(v2.ok());
  auto v3 = executor->CreateCall(v2->ref(), v1->ref());
  EXPECT_TRUE(v3.ok());
  const std::vector<ValueId> elements = {v1->ref(), v3->ref()};
  auto v4 = executor->CreateStruct(absl::MakeSpan(elements));
  EXPECT_TRUE(v4.ok());
  auto v5 = executor->CreateSelection(v4->ref(), 1);
  EXPECT_TRUE(v5.ok());
  v0::Value result_pb;
  EXPECT_TRUE(executor->Materialize(v5.value().ref(), &result_pb).ok());
  EXPECT_EQ(result_pb.str(), "(some_arg,some_func(some_arg))[1]");
}

}  // namespace generative_computing
