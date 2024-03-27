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

#include "generative_computing/cc/types/inference.h"

#include "googletest/include/gtest/gtest.h"
#include "absl/status/status.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace {

class InferenceTest : public ::testing::Test {
 protected:
  InferenceTest() {}
  ~InferenceTest() override {}
};

TEST_F(InferenceTest, Scalar) {
  v0::Value val_pb;
  val_pb.set_str("bark");
  absl::Status status = InferTypes(&val_pb);
  EXPECT_TRUE(status.ok());
  EXPECT_TRUE(val_pb.has_type());
  EXPECT_TRUE(val_pb.type().has_scalar());
  EXPECT_EQ(val_pb.type().scalar(), v0::SCALAR_TYPE_STRING);
}

TEST_F(InferenceTest, ScalarWithCorrectTypePreset) {
  v0::Value val_pb;
  val_pb.set_str("bark");
  val_pb.mutable_type()->set_scalar(v0::SCALAR_TYPE_STRING);
  absl::Status status = InferTypes(&val_pb);
  EXPECT_TRUE(status.ok());
  EXPECT_TRUE(val_pb.has_type());
  EXPECT_TRUE(val_pb.type().has_scalar());
  EXPECT_EQ(val_pb.type().scalar(), v0::SCALAR_TYPE_STRING);
}

TEST_F(InferenceTest, ScalarWithIncorrectTypePreset) {
  v0::Value val_pb;
  val_pb.set_str("bark");
  val_pb.mutable_type()->set_scalar(v0::SCALAR_TYPE_INT32);
  absl::Status status = InferTypes(&val_pb);
  EXPECT_TRUE(status.code() == absl::StatusCode::kInvalidArgument);
}

TEST_F(InferenceTest, StructWithOneElement) {
  v0::Value val_pb;
  val_pb.mutable_struct_()->add_element()->set_str("bark");
  absl::Status status = InferTypes(&val_pb);
  EXPECT_TRUE(status.ok());
  v0::Type expected_type;
  expected_type.mutable_struct_()->add_element()->set_scalar(
      v0::SCALAR_TYPE_STRING);
  EXPECT_EQ(val_pb.type().DebugString(), expected_type.DebugString());
  EXPECT_EQ(val_pb.struct_().element(0).type().DebugString(),
            expected_type.struct_().element(0).DebugString());
}

TEST_F(InferenceTest, StructWithTwoElements) {
  v0::Value val_pb;
  val_pb.mutable_struct_()->add_element()->set_str("bark");
  val_pb.mutable_struct_()->add_element()->set_int_32(10);
  absl::Status status = InferTypes(&val_pb);
  EXPECT_TRUE(status.ok());
  v0::Type expected_type;
  expected_type.mutable_struct_()->add_element()->set_scalar(
      v0::SCALAR_TYPE_STRING);
  expected_type.mutable_struct_()->add_element()->set_scalar(
      v0::SCALAR_TYPE_INT32);
  EXPECT_EQ(val_pb.type().DebugString(), expected_type.DebugString());
  EXPECT_EQ(val_pb.struct_().element(0).type().DebugString(),
            expected_type.struct_().element(0).DebugString());
  EXPECT_EQ(val_pb.struct_().element(1).type().DebugString(),
            expected_type.struct_().element(1).DebugString());
}

TEST_F(InferenceTest, NestedStruct) {
  v0::Value val_pb;
  val_pb.mutable_struct_()->add_element()->set_str("bark");
  v0::Struct* const s =
      val_pb.mutable_struct_()->add_element()->mutable_struct_();
  s->add_element()->set_int_32(10);
  s->add_element()->set_str("oink");
  absl::Status status = InferTypes(&val_pb);
  EXPECT_TRUE(status.ok());
  v0::Type expected_type;
  expected_type.mutable_struct_()->add_element()->set_scalar(
      v0::SCALAR_TYPE_STRING);
  v0::StructType* const st =
      expected_type.mutable_struct_()->add_element()->mutable_struct_();
  st->add_element()->set_scalar(v0::SCALAR_TYPE_INT32);
  st->add_element()->set_scalar(v0::SCALAR_TYPE_STRING);
  EXPECT_EQ(val_pb.type().DebugString(), expected_type.DebugString());
  EXPECT_EQ(val_pb.struct_().element(0).type().DebugString(),
            expected_type.struct_().element(0).DebugString());
  EXPECT_EQ(val_pb.struct_().element(1).type().DebugString(),
            expected_type.struct_().element(1).DebugString());
}

TEST_F(InferenceTest, Selection) {
  v0::Value val_pb;
  val_pb.mutable_selection()->set_index(1);
  v0::Struct* const s =
      val_pb.mutable_selection()->mutable_source()->mutable_struct_();
  s->add_element()->set_str("bark");
  s->add_element()->set_int_32(10);
  absl::Status status = InferTypes(&val_pb);
  EXPECT_TRUE(status.ok());
  v0::Type expected_type;
  expected_type.set_scalar(v0::SCALAR_TYPE_INT32);
  EXPECT_EQ(val_pb.type().DebugString(), expected_type.DebugString());
}

}  // namespace
}  // namespace generative_computing
