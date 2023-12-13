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

#include "generative_computing/cc/testing/testing_libs.h"

#include <string>

#include "googletest/include/gtest/gtest.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {

TEST(FormatValueAsStringTest, FormatsStructCorrectly) {
  v0::Value value;
  v0::Struct* strcut_input = value.mutable_struct_();
  strcut_input->add_element()->set_str("str");
  strcut_input->add_element()->set_boolean(true);
  strcut_input->add_element()->set_media("media");
  strcut_input->add_element()->set_int_32(123);

  std::string result = testing::FormatValueAsString(value).value();
  EXPECT_EQ(result, "str,true,media,123");
}

TEST(WrapFnNameAroundValueTest, WrapsFnNameAroundValueCorrectly) {
  v0::Value value;
  v0::Struct* strcut_input = value.mutable_struct_();
  strcut_input->add_element()->set_str("str");
  strcut_input->add_element()->set_boolean(true);

  v0::Value result = testing::WrapFnNameAroundValue("fn", value).value();
  EXPECT_EQ(result.str(), "fn(str,true)");
}

}  // namespace
}  // namespace generative_computing
