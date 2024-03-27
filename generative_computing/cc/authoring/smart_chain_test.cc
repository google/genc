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

#include "generative_computing/cc/authoring/smart_chain.h"

#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/authoring/constructor.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {

TEST(SmartChainTest, CanBuildsRepeatedConditionalChain) {
  v0::Value append_foo_fn = CreateCustomFunction("append_foo").value();
  v0::Value append_bar_fn = CreateCustomFunction("append_bar").value();
  v0::Value if_finish_then_break_fn = CreateRegexPartialMatch("FINISH").value();
  v0::Value count_to_3_append_finish_fn =
      CreateCustomFunction("append_finish_when_counts_reaches_3").value();
  v0::Value expected_computation =
      CreateRepeatedConditionalChain(
          100, {append_foo_fn, if_finish_then_break_fn, append_bar_fn,
                count_to_3_append_finish_fn})
          .value();

  SmartChain smart_chain = SmartChain() | append_foo_fn |
                           if_finish_then_break_fn | append_bar_fn |
                           count_to_3_append_finish_fn;
  smart_chain.SetNumIteration(100);
  v0::Value computation = smart_chain.Build().value();
  EXPECT_EQ(computation.DebugString(), expected_computation.DebugString());
}

TEST(SmartChainTest, BuildsParallelChain) {
  v0::Value append_foo_fn = CreateModelInference("append_foo").value();
  v0::Value append_bar_fn = CreateModelInference("append_bar").value();

  SmartChain smart_chain = SmartChain() | append_foo_fn | append_bar_fn;
  smart_chain.SetIsParallel(true);
  v0::Value computation = smart_chain.Build().value();

  v0::Value serial_chain =
      CreateSerialChain({append_foo_fn, append_bar_fn}).value();
  v0::Value expected_computation = CreateParallelMap(serial_chain).value();

  EXPECT_EQ(computation.DebugString(), expected_computation.DebugString());
}

}  // namespace
}  // namespace generative_computing
