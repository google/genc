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
#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "genc/cc/authoring/constructor.h"
#include "genc/cc/intrinsics/handler_sets.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/inline_executor.h"
#include "genc/cc/runtime/runner.h"
#include "genc/cc/runtime/threading.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace {

TEST(InjaTemplateTest, ProcessesInputTemplateSuccessfully) {
  std::shared_ptr<Executor> executor =
      CreateInlineExecutor(intrinsics::CreateCompleteHandlerSet({}),
                           CreateThreadBasedConcurrencyManager())
          .value();
  Runner runner = Runner::Create(executor).value();

  v0::Value template_pb = CreateInjaTemplate(
                              "{% for f in flights %}{{f.origin}} to "
                              "{{f.destination}}:{{f.price}}\n{% endfor %}")
                              .value();

  v0::Value arg_pb;
  arg_pb.set_str(R"(
  {"flights":[
    {
      "origin": "SFO",
      "destination": "LAX",
      "price": 1
    },
    {
      "origin": "SFO",
      "destination": "JFK",
      "price": 2
    }
  ]}
  )");

  v0::Value result = runner.Run(template_pb, arg_pb).value();
  EXPECT_EQ(result.str(), "SFO to LAX:1\nSFO to JFK:2\n");
}
}  // namespace
}  // namespace genc
