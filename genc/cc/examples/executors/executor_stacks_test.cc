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

#include <future>  // NOLINT
#include <memory>
#include <thread>  // NOLINT
#include <utility>
#include <vector>
#include "googletest/include/gtest/gtest.h"
#include "absl/status/status.h"
#include "genc/cc/examples/executors/executor_stacks.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace {

TEST(ExecutorStacksTest, CreatesManySequentially) {
  for (int i = 0; i < 100000; ++i) {
    EXPECT_TRUE(CreateDefaultExecutor().ok());
  }
}

TEST(ExecutorStacksTest, CreatesAndRunsManyConcurrently) {
  std::vector<std::shared_future<absl::Status>> futures;
  futures.reserve(10000);
  for (int i = 0; i < 10000; ++i) {
    std::packaged_task<absl::Status()> task([i]() -> absl::Status {
      auto executor = GENC_TRY(CreateDefaultExecutor());
      v0::Value arg;
      arg.set_int_32(i);
      OwnedValueId arg_val = GENC_TRY(executor->CreateValue(arg));
      v0::Value result;
      GENC_TRY(executor->Materialize(arg_val.ref(), &result));
      if (!result.has_int_32()) {
        return absl::InternalError("Expected int32 result");
      }
      if (result.int_32() != i) {
        return absl::InternalError("Wrong result value.");
      }
      return absl::OkStatus();
    });
    auto future_ptr = std::shared_future<absl::Status>(task.get_future());
    std::thread th(std::move(task));
    th.detach();
    futures.push_back(std::move(future_ptr));
  }
  for (const auto& future : futures) {
    EXPECT_TRUE(future.get().ok());
  }
}

}  // namespace
}  // namespace genc
