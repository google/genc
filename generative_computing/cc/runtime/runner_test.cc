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

#include "generative_computing/cc/runtime/runner.h"

#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/authoring/constructor.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

TEST(RunnerTest, ModelRunReturnsValue) {
  v0::Value comp_pb = CreateModelInference("test_model").value();
  absl::StatusOr<Runner> runner = Runner::CreateDefault(comp_pb);

  v0::Value arg;
  arg.set_str("Boo!");

  absl::StatusOr<v0::Value> result = runner.value().Run(arg);
  EXPECT_EQ(result.value().str(),
            "This is an output from a test model in response to \"Boo!\".");
}

}  // namespace generative_computing
