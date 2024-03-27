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

#include "generative_computing/cc/types/checking.h"

#include "googletest/include/gtest/gtest.h"
#include "absl/status/status.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace {

class CheckingTest : public ::testing::Test {
 protected:
  CheckingTest() {}
  ~CheckingTest() override {}
};

TEST_F(CheckingTest, EqualScalars) {
  v0::Type x, y;
  x.set_scalar(v0::SCALAR_TYPE_INT32);
  y.set_scalar(v0::SCALAR_TYPE_INT32);
  absl::Status status = CheckEqual(x, y);
  EXPECT_TRUE(status.ok());
}

TEST_F(CheckingTest, UnequalScalars) {
  v0::Type x, y;
  x.set_scalar(v0::SCALAR_TYPE_INT32);
  y.set_scalar(v0::SCALAR_TYPE_STRING);
  absl::Status status = CheckEqual(x, y);
  EXPECT_TRUE(status.code() == absl::StatusCode::kInvalidArgument);
}

TEST_F(CheckingTest, ScalarAndStruct) {
  v0::Type x, y;
  x.set_scalar(v0::SCALAR_TYPE_INT32);
  y.mutable_struct_()->add_element()->set_scalar(v0::SCALAR_TYPE_INT32);
  absl::Status status = CheckEqual(x, y);
  EXPECT_TRUE(status.code() == absl::StatusCode::kInvalidArgument);
}

TEST_F(CheckingTest, StructAndScalar) {
  v0::Type x, y;
  x.mutable_struct_()->add_element()->set_scalar(v0::SCALAR_TYPE_INT32);
  y.set_scalar(v0::SCALAR_TYPE_INT32);
  absl::Status status = CheckEqual(x, y);
  EXPECT_TRUE(status.code() == absl::StatusCode::kInvalidArgument);
}

TEST_F(CheckingTest, StructsOfUnequalSizes) {
  v0::Type x, y;
  x.mutable_struct_()->add_element()->set_scalar(v0::SCALAR_TYPE_INT32);
  y.mutable_struct_()->add_element()->set_scalar(v0::SCALAR_TYPE_INT32);
  y.mutable_struct_()->add_element()->set_scalar(v0::SCALAR_TYPE_INT32);
  absl::Status status = CheckEqual(x, y);
  EXPECT_TRUE(status.code() == absl::StatusCode::kInvalidArgument);
}

TEST_F(CheckingTest, StructsWithMismatchingElementTypes) {
  v0::Type x, y;
  x.mutable_struct_()->add_element()->set_scalar(v0::SCALAR_TYPE_INT32);
  y.mutable_struct_()->add_element()->set_scalar(v0::SCALAR_TYPE_STRING);
  absl::Status status = CheckEqual(x, y);
  EXPECT_TRUE(status.code() == absl::StatusCode::kInvalidArgument);
}

TEST_F(CheckingTest, EqualStructs) {
  v0::Type x, y;
  x.mutable_struct_()->add_element()->set_scalar(v0::SCALAR_TYPE_INT32);
  y.mutable_struct_()->add_element()->set_scalar(v0::SCALAR_TYPE_INT32);
  absl::Status status = CheckEqual(x, y);
  EXPECT_TRUE(status.ok());
}

TEST_F(CheckingTest, EqualFunctions) {
  v0::Type x, y;
  x.mutable_function()->mutable_parameter()->set_scalar(v0::SCALAR_TYPE_INT32);
  x.mutable_function()->mutable_result()->set_scalar(v0::SCALAR_TYPE_INT32);
  y.mutable_function()->mutable_parameter()->set_scalar(v0::SCALAR_TYPE_INT32);
  y.mutable_function()->mutable_result()->set_scalar(v0::SCALAR_TYPE_INT32);
  absl::Status status = CheckEqual(x, y);
  EXPECT_TRUE(status.ok());
}

TEST_F(CheckingTest, FunctionsWithMismatchingParameters) {
  v0::Type x, y;
  x.mutable_function()->mutable_parameter()->set_scalar(v0::SCALAR_TYPE_INT32);
  x.mutable_function()->mutable_result()->set_scalar(v0::SCALAR_TYPE_INT32);
  y.mutable_function()->mutable_parameter()->set_scalar(v0::SCALAR_TYPE_STRING);
  y.mutable_function()->mutable_result()->set_scalar(v0::SCALAR_TYPE_INT32);
  absl::Status status = CheckEqual(x, y);
  EXPECT_TRUE(status.code() == absl::StatusCode::kInvalidArgument);
}

TEST_F(CheckingTest, FunctionsWithMismatchingResults) {
  v0::Type x, y;
  x.mutable_function()->mutable_parameter()->set_scalar(v0::SCALAR_TYPE_INT32);
  x.mutable_function()->mutable_result()->set_scalar(v0::SCALAR_TYPE_INT32);
  y.mutable_function()->mutable_parameter()->set_scalar(v0::SCALAR_TYPE_INT32);
  y.mutable_function()->mutable_result()->set_scalar(v0::SCALAR_TYPE_STRING);
  absl::Status status = CheckEqual(x, y);
  EXPECT_TRUE(status.code() == absl::StatusCode::kInvalidArgument);
}

}  // namespace
}  // namespace generative_computing
