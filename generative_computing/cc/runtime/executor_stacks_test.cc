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

#include "generative_computing/cc/runtime/executor_stacks.h"

#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/runtime/executor.h"

namespace generative_computing {
namespace {

class ExecutorStacksTest : public ::testing::Test {
 protected:
  ExecutorStacksTest() {}
  ~ExecutorStacksTest() override {}
};

TEST_F(ExecutorStacksTest, Simple) {
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      generative_computing::CreateDefaultLocalExecutor();
  EXPECT_TRUE(executor.ok());
}

}  // namespace
}  // namespace generative_computing
