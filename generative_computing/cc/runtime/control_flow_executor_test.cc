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

#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/intrinsics/handler_sets.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/inline_executor.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"

namespace generative_computing {

class ControlFlowExecutorTest : public ::testing::Test {
 protected:
  ControlFlowExecutorTest() {}
  ~ControlFlowExecutorTest() override {}
};

TEST_F(ControlFlowExecutorTest, Simple) {
  const std::shared_ptr<IntrinsicHandlerSet> handler_set =
      intrinsics::CreateCompleteHandlerSet({});
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      CreateControlFlowExecutor(handler_set,
                                CreateInlineExecutor(handler_set).value());
  EXPECT_TRUE(executor.ok());

  // TODO(b/295041821): Pull in the tests.
}

}  // namespace generative_computing
