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

#include "generative_computing/cc/authoring/constructor.h"

#include <string>

#include "googletest/include/gtest/gtest.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {

absl::StatusOr<absl::flat_hash_map<std::string, v0::Value>>
ExtractStaticParameters(const v0::Value& value) {
  absl::flat_hash_map<std::string, v0::Value> kwargs;
  for (const auto& arg :
       value.intrinsic().static_parameter().struct_().element()) {
    kwargs.insert({arg.name(), arg.value()});
  }
  return kwargs;
}

TEST(CreateRepeatTest, ReturnsCorrectRepeatProto) {
  int steps = 5;
  std::string test_fn_name = "test_body_fn";
  v0::Value repeat_pb = CreateRepeat(steps, test_fn_name).value();
  EXPECT_EQ(repeat_pb.intrinsic().uri(), "repeat");
  absl::flat_hash_map<std::string, v0::Value> kwargs =
      ExtractStaticParameters(repeat_pb).value();

  EXPECT_EQ(kwargs.at("num_steps").int_32(), steps);
  EXPECT_EQ(kwargs.at("body_fn").intrinsic().static_parameter().str(),
            test_fn_name);
}

TEST(CreateModelInferenceTest, ReturnsCorrectModelInferenceProto) {
  std::string test_model_uri = "test_model_uri";
  v0::Value model_pb = CreateModelInference(test_model_uri).value();
  EXPECT_EQ(model_pb.intrinsic().uri(), "model_inference");
  EXPECT_EQ(model_pb.intrinsic().static_parameter().str(), test_model_uri);
}
}  // namespace
}  // namespace generative_computing
