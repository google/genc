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

#include "genc/cc/base/computation.h"

#include <memory>
#include <vector>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "genc/cc/base/context.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace {

class TestContext : public Context {
 public:
  absl::StatusOr<v0::Value> Call(const v0::Value& portable_ir,
                                 const std::vector<v0::Value>& args) override {
    v0::Value resposne;
    resposne.set_str(portable_ir.str() +
                     (args.empty() ? "" : "(" + args[0].str() + ")"));
    return resposne;
  }

  ~TestContext() override {}
};

TEST(ComputationTest, IsCallable) {
  GetContextStack()->SetDefaultContext(std::make_shared<TestContext>());

  v0::Value const_computation;
  const_computation.set_str("function");

  v0::Value arg;
  arg.set_str("argument");

  Computation computation(const_computation);
  Computation result = computation({arg}).value();
  EXPECT_EQ(result.portable_ir().str(), "function(argument)");
}
}  // namespace
}  // namespace genc
