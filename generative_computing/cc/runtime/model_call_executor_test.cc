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

#include "generative_computing/cc/runtime/model_call_executor.h"
#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"

namespace generative_computing{

class ModelCallExecutorTest : public ::testing::Test {
 protected:
  ModelCallExecutorTest() {}
  ~ModelCallExecutorTest() override {}

  // TODO(b/295260921): This needs to test a setup with multiple executors that
  // handle models for various URI prefixes, that calls are routed to an
  // appropriate child executor depending on the model URI, and that bogus URIs
  // result in an error.
};

TEST_F(ModelCallExecutorTest, Simple) {}

}  // namespace generative_computing
