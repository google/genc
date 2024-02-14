#include "generative_computing/cc/modules/parsers/gemini_parser.h"
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


#include "googletest/include/gtest/gtest.h"
#include "generative_computing/proto/v0/computation.pb.h"
#include <nlohmann/json.hpp>

namespace generative_computing {

namespace {

TEST(WrapTextAsInputJsonTest, OnSuccessProducesTheCorrectJson) {
  v0::Value input_pb;
  input_pb.set_str("test_query");
  v0::Value json_value = GeminiParser::WrapTextAsInputJson(input_pb).value();
  auto actual_json = nlohmann::json::parse(json_value.str());

  auto expected_json = nlohmann::json::parse(
      R"pb(
        {
          "contents":
          [ {
            "parts":
            [ { "text": "test_query" }]
          }]
        }
      )pb");
  EXPECT_TRUE(actual_json == expected_json);
}

TEST(GetTopCandidateAsTextTest, OnSuccessExtractsTheCorrectText) {
  v0::Value model_output_pb;
  model_output_pb.set_str(R"pb(
    {
      "candidates":
      [ {
        "content": {
          "parts":
          [ { "text": "Once upon a time" }
            , { "text": "..." }],
          "role": "model"
        },
        "finishReason": "STOP"
      }]
    }
  )pb");

  v0::Value parsed_output =
      GeminiParser::GetTopCandidateAsText(model_output_pb).value();
  EXPECT_EQ(parsed_output.str(), "Once upon a time...");
}

}  // namespace
}  // namespace generative_computing
