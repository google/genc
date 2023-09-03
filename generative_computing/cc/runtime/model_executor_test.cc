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

#include "generative_computing/cc/runtime/model_executor.h"

#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/proto/v0/computation.pb.h"
#include "generative_computing/proto/v0/executor.pb.h"

namespace generative_computing{

class ModelExecutorTest : public ::testing::Test {
 protected:
  ModelExecutorTest() {}
  ~ModelExecutorTest() override {}

  // TODO(b/295260921): This needs to test a setup with multiple executors that
  // handle models for various URI prefixes, that calls are routed to an
  // appropriate child executor depending on the model URI, and that bogus URIs
  // result in an error.
};

TEST_F(ModelExecutorTest, TestModel) {
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      generative_computing::CreateModelExecutor();
  EXPECT_TRUE(executor.ok());

  v0::Value fn_pb;
  fn_pb.mutable_computation()->mutable_model()->mutable_model_id()->set_uri(
      "test_model");
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

TEST_F(ModelExecutorTest, PromptTemplate) {
  absl::StatusOr<std::shared_ptr<Executor>> executor =
      generative_computing::CreateModelExecutor();
  EXPECT_TRUE(executor.ok());

  v0::Value fn_pb;
  fn_pb.mutable_computation()->mutable_prompt_template()->set_template_string(
      "Q: What should I pack for a trip to {location}? A: ");
  absl::StatusOr<OwnedValueId> fn_val = executor.value()->CreateValue(fn_pb);
  EXPECT_TRUE(fn_val.ok());

  v0::Value arg_pb;
  arg_pb.set_str("a grocery store");
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
            "Q: What should I pack for a trip to a grocery store? A: ");
}

}  // namespace generative_computing
