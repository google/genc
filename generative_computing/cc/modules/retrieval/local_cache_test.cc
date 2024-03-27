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

#include "generative_computing/cc/modules/retrieval/local_cache.h"

#include <functional>
#include <future>  // NOLINT
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "googlemock/include/gmock/gmock.h"
#include "googletest/include/gtest/gtest.h"
#include "absl/status/status.h"
#include "generative_computing/cc/authoring/constructor.h"
#include "generative_computing/cc/intrinsics/handler_sets.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/inline_executor.h"
#include "generative_computing/cc/runtime/runner.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {

TEST(GetTest, OnSuccessRetrievesMessages) {
  LocalStringCache cache(3);
  cache.Put("key", "v1");
  cache.Put("key", "v2");
  cache.Put("key", "v3");
  auto values = cache.Get("key");
  ASSERT_EQ(values.size(), 3);
  ASSERT_EQ(values[0], "v1");
  ASSERT_EQ(values[1], "v2");
  ASSERT_EQ(values[2], "v3");
}

TEST(PutTest, PrunesEarliestMessageWhenCapacityReached) {
  LocalStringCache cache(2);

  cache.Put("key", "v1");
  cache.Put("key", "v2");
  // Capacity reached, will prune like a FIFO.
  cache.Put("key", "v3");

  auto values = cache.Get("key");
  ASSERT_EQ(values.size(), 2);
  ASSERT_EQ(values[0], "v2");
  ASSERT_EQ(values[1], "v3");
}

TEST(RemoveTest, DeletesAllMessages) {
  LocalStringCache cache(2);
  cache.Put("key", "v1");
  cache.Put("key", "v2");
  cache.Remove("key");

  ASSERT_FALSE(cache.Exists("key"));
  auto values = cache.Get("key");
  ASSERT_TRUE(values.empty());
}

TEST(ThreadSafetyTest, MultiTreadedReadWriteIsSafe) {
  LocalCache<std::string, int> cache(100);

  const std::function<void()> perform_read_write = [&cache]() {
    for (int i = 1; i <= 10; ++i) {
      cache.Put("key", i);
      // read last message
      cache.Get("key", 1);
    }
  };

  std::vector<std::future<void>> futures;

  for (int i = 0; i < 5; i++) {
    futures.push_back(std::async(std::launch::async, perform_read_write));
  }

  for (auto& future : futures) {
    future.wait();
  }

  auto numbers = cache.Get("key");
  int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
  // Each thread wrote 1-10 into cache, which sums up to 55; 5 threads in total;
  // So sum should be 55*5 = 275.
  EXPECT_EQ(sum, 275);
}

TEST(SetCustomFunctionsForLocalValueCache, CacheReadWriteWorksWithExecutor) {
  intrinsics::HandlerSetConfig config;
  LocalValueCache cache = LocalValueCache(100);
  EXPECT_EQ(
      SetCustomFunctionsForLocalValueCache(config.custom_function_map,
      cache), absl::OkStatus());
  std::shared_ptr<Executor> executor =
      CreateInlineExecutor(intrinsics::CreateCompleteHandlerSet(config))
          .value();

  v0::Value read_pb = CreateCustomFunction(kLocalCacheReadUri).value();
  v0::Value write_pb = CreateCustomFunction(kLocalCacheWriteUri).value();

  Runner runner = Runner::Create(executor).value();

  v0::Value test_input;
  test_input.set_str("test");
  v0::Value read_all;
  runner.Run(write_pb, test_input).value();
  runner.Run(write_pb, test_input).value();
  v0::Value result = runner.Run(read_pb, read_all).value();
  EXPECT_EQ(result.str(), "test\ntest\n");
}
}  // namespace
}  // namespace generative_computing
