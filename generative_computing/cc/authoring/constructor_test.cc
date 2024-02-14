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

#include "testing/base/public/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "generative_computing/proto/v0/computation.pb.h"

using testing::EqualsProto;

namespace generative_computing {

namespace {

absl::StatusOr<absl::flat_hash_map<std::string, v0::Value>>
ExtractStaticParameters(const v0::Value& value) {
  absl::flat_hash_map<std::string, v0::Value> kwargs;
  for (const auto& arg :
       value.intrinsic().static_parameter().struct_().element()) {
    kwargs.insert({arg.label(), arg});
  }
  return kwargs;
}

TEST(CreateRepeatTest, ReturnsCorrectRepeatProto) {
  int steps = 5;
  v0::Value model_pb = CreateModelInference("test_model_uri").value();
  v0::Value repeat_pb = CreateRepeat(steps, model_pb).value();
  EXPECT_EQ(repeat_pb.intrinsic().uri(), "repeat");
  absl::flat_hash_map<std::string, v0::Value> kwargs =
      ExtractStaticParameters(repeat_pb).value();

  EXPECT_EQ(kwargs.at("num_steps").int_32(), steps);
  EXPECT_EQ(kwargs.at("body_fn").intrinsic().static_parameter().str(),
            model_pb.intrinsic().static_parameter().str());
}

TEST(CreateModelInferenceTest, ReturnsCorrectModelInferenceProto) {
  std::string test_model_uri = "test_model_uri";
  v0::Value model_pb = CreateModelInference(test_model_uri).value();
  EXPECT_EQ(model_pb.intrinsic().uri(), "model_inference");
  EXPECT_EQ(model_pb.intrinsic().static_parameter().str(), test_model_uri);
}

TEST(CreateModelInferenceWithConfigTest,
     ReturnsCorrectModelInferenceWithStructConfigProto) {
  std::string test_model_uri = "test_model_uri";
  std::string test_config_1 =  "test_config_1";
  std::string test_config_2 =  "test_config_2";

  v0::Value model_config_pb;
  v0::Struct* model_config_struct =
      model_config_pb.mutable_struct_();
  v0::Value* element_1 = model_config_struct->add_element();
  element_1->set_label("config_1");
  element_1->set_str(test_config_1);
  v0::Value* element_2 = model_config_struct->add_element();
  element_2->set_label("config_2");
  element_2->set_str(test_config_2);

  v0::Value model_pb = CreateModelInferenceWithConfig(
      test_model_uri, model_config_pb).value();
  EXPECT_EQ(model_pb.intrinsic().uri(), "model_inference_with_config");

  EXPECT_EQ(
      model_pb.intrinsic().static_parameter().struct_().element(0).label(),
      "model_uri");
  EXPECT_EQ(
      model_pb.intrinsic().static_parameter().struct_().element(0).str(),
      test_model_uri);
  EXPECT_EQ(
      model_pb.intrinsic().static_parameter().struct_().element(1).label(),
      "model_config");
  EXPECT_THAT(
      model_pb.intrinsic().static_parameter().struct_().element(1).struct_(),
      EqualsProto(model_config_pb.struct_()));
}

TEST(CreateModelInferenceWithConfigTest,
     ReturnsCorrectModelInferenceWithStringConfigProto) {
  std::string test_model_uri = "test_model_uri";
  std::string test_config_str =  "test_config";

  v0::Value model_config_pb;
  model_config_pb.set_str(test_config_str);

  v0::Value model_pb = CreateModelInferenceWithConfig(
      test_model_uri, model_config_pb).value();
  EXPECT_EQ(model_pb.intrinsic().uri(), "model_inference_with_config");

  EXPECT_EQ(
      model_pb.intrinsic().static_parameter().struct_().element(0).label(),
      "model_uri");
  EXPECT_EQ(
      model_pb.intrinsic().static_parameter().struct_().element(0).str(),
      test_model_uri);
  EXPECT_EQ(
      model_pb.intrinsic().static_parameter().struct_().element(1).label(),
      "model_config");
  EXPECT_THAT(
      model_pb.intrinsic().static_parameter().struct_().element(1).str(),
      test_config_str);
}

TEST(CreateCustomFunctionTest, ReturnsCorrectCustomFunctionProto) {
  std::string fn_uri = "test_fn_uri";
  v0::Value custom_fn_pb = CreateCustomFunction(fn_uri).value();
  EXPECT_EQ(custom_fn_pb.intrinsic().uri(), "custom_function");
  EXPECT_EQ(custom_fn_pb.intrinsic().static_parameter().str(), fn_uri);
}

TEST(CreateWhileTest, ReturnsCorrectLogicalNotProto) {
  v0::Value logical_not_pb = CreateLogicalNot().value();
  EXPECT_EQ(logical_not_pb.intrinsic().uri(), "logical_not");
}

TEST(CreateWhileTest, ReturnsCorrectRegexPartialMatchProto) {
  std::string test_regex_pattern = "test_pattern";
  v0::Value regex_pb = CreateRegexPartialMatch(test_regex_pattern).value();
  EXPECT_EQ(regex_pb.intrinsic().uri(), "regex_partial_match");
  EXPECT_EQ(regex_pb.intrinsic().static_parameter().str(), test_regex_pattern);
}

TEST(CreateWhileTest, ReturnsCorrectWhileProto) {
  v0::Value test_condition_fn =
      CreateRegexPartialMatch("stop_keyword: Finish").value();
  v0::Value test_body_fn = CreateModelInference("test_model").value();
  v0::Value while_pb = CreateWhile(test_condition_fn, test_body_fn).value();
  EXPECT_EQ(while_pb.intrinsic().uri(), "while");
  absl::flat_hash_map<std::string, v0::Value> kwargs =
      ExtractStaticParameters(while_pb).value();

  EXPECT_EQ(kwargs.at("condition_fn").intrinsic().uri(), "regex_partial_match");
  EXPECT_EQ(kwargs.at("body_fn").intrinsic().uri(), "model_inference");
}

TEST(CreateInjaTempalte, ReturnsCorrectComputationProto) {
  std::string test_template = "test_template";
  v0::Value inja_pb = CreateInjaTemplate(test_template).value();
  EXPECT_EQ(inja_pb.intrinsic().uri(), "inja_template");
  EXPECT_EQ(inja_pb.intrinsic().static_parameter().str(), test_template);
}

TEST(CreateRestCall, ReturnsCorrectComputationProto) {
  std::string test_api_key = "test_api_key";
  std::string test_uri = "https://test/uri";
  v0::Value rest_call_pb = CreateRestCall(test_uri, test_api_key).value();
  EXPECT_EQ(rest_call_pb.intrinsic().uri(), "rest_call");
  EXPECT_EQ(
      rest_call_pb.intrinsic().static_parameter().struct_().element(0).str(),
      "POST");
  EXPECT_EQ(
      rest_call_pb.intrinsic().static_parameter().struct_().element(1).str(),
      test_uri);
  EXPECT_EQ(
      rest_call_pb.intrinsic().static_parameter().struct_().element(2).str(),
      test_api_key);
}

TEST(CreateWolframAlpha, ReturnsCorrectComputationProto) {
  std::string test_appid = "test_appid";

  v0::Value wolfram_alpha_pb = CreateWolframAlpha(test_appid).value();
  EXPECT_EQ(wolfram_alpha_pb.intrinsic().uri(), "wolfram_alpha");
  EXPECT_EQ(wolfram_alpha_pb.intrinsic().static_parameter().str(), test_appid);
}

}  // namespace
}  // namespace generative_computing
