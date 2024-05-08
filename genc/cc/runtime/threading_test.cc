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

#include "genc/cc/runtime/threading.h"

#include <memory>

#include "googletest/include/gtest/gtest.h"
#include "absl/status/statusor.h"
#include "genc/cc/runtime/status_macros.h"

namespace genc {
namespace {

TEST(ThreadingTest, OneCallback) {
  auto cc = CreateThreadBasedConcurrencyManager();
  auto future = cc->RunAsync([]() -> int { return 10; });
  absl::StatusOr<int> result = future->Get();
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.value(), 10);
}

TEST(ThreadingTest, TwoParallelCallbacks) {
  auto cc = CreateThreadBasedConcurrencyManager();
  auto future_1 = cc->RunAsync([]() -> int { return 10; });
  auto future_2 = cc->RunAsync([]() -> int { return 20; });
  absl::StatusOr<int> result_1 = future_1->Get();
  EXPECT_TRUE(result_1.ok());
  EXPECT_EQ(result_1.value(), 10);
  absl::StatusOr<int> result_2 = future_2->Get();
  EXPECT_TRUE(result_2.ok());
  EXPECT_EQ(result_2.value(), 20);
}

TEST(ThreadingTest, ParentAndTwoChildCallbacks) {
  auto cc = CreateThreadBasedConcurrencyManager();
  auto future = cc->RunAsync([cc]() -> absl::StatusOr<int> {
    auto f1 = cc->RunAsync([]() -> int { return 10; });
    auto f2 = cc->RunAsync([]() -> int { return 20; });
    return GENC_TRY(f1->Get()) + GENC_TRY(f2->Get());
  });
  auto result = future->Get();
  EXPECT_TRUE(result.ok());
  EXPECT_TRUE(result.value().ok());
  EXPECT_EQ(result->value(), 30);
}

}  // namespace
}  // namespace genc
