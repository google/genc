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

#include "genc/cc/runtime/runner.h"

#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "genc/cc/authoring/constructor.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/executor_stacks.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

TEST(RunnerTest, ModelRunReturnsValue) {
  v0::Value comp_pb = CreateModelInference("test_model").value();
  Runner runner = Runner::CreateDefault().value();

  v0::Value arg;
  arg.set_str("Boo!");

  absl::StatusOr<v0::Value> result = runner.Run(comp_pb, arg);
  EXPECT_EQ(result.value().str(),
            "This is an output from a test model in response to \"Boo!\".");
}

TEST(RunnerTest, ModelRunReturnsValueCompInConstructor) {
  v0::Value comp_pb = CreateModelInference("test_model").value();
  Runner runner = Runner::Create(comp_pb).value();

  v0::Value arg;
  arg.set_str("Boo!");

  absl::StatusOr<v0::Value> result = runner.Run(arg);
  EXPECT_EQ(result.value().str(),
            "This is an output from a test model in response to \"Boo!\".");
}

TEST(RunnerTest, ModelRunReturnsValueRuntimeInConstructor) {
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      CreateDefaultLocalExecutor();
  EXPECT_TRUE(executor.ok());
  v0::Value comp_pb = CreateModelInference("test_model").value();
  Runner runner = Runner::Create(executor.value()).value();

  v0::Value arg;
  arg.set_str("Boo!");

  absl::StatusOr<v0::Value> result = runner.Run(comp_pb, arg);
  EXPECT_EQ(result.value().str(),
            "This is an output from a test model in response to \"Boo!\".");
}

TEST(RunnerTest, ModelRunReturnsValueCompAndRuntimeInConstructor) {
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      CreateDefaultLocalExecutor();
  EXPECT_TRUE(executor.ok());
  v0::Value comp_pb = CreateModelInference("test_model").value();
  Runner runner = Runner::Create(comp_pb, executor.value()).value();

  v0::Value arg;
  arg.set_str("Boo!");

  absl::StatusOr<v0::Value> result = runner.Run(arg);
  EXPECT_EQ(result.value().str(),
            "This is an output from a test model in response to \"Boo!\".");
}

}  // namespace genc
