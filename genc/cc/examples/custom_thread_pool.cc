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

#include <functional>
#include <iostream>
#include <memory>
#include <thread>  // NOLINT
#include <utility>

#include "absl/status/status.h"
#include "genc/cc/authoring/constructor.h"
#include "genc/cc/examples/executors/executor_stacks.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/runner.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/cc/runtime/threading.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace {

class MyCustomThreadPool : public ThreadPool {
 public:
  MyCustomThreadPool() {}

  virtual absl::Status Schedule(std::function<void()> task) override {
    // This is the part you would replace with your custom thread scheduling
    // functionality. For demonstration purposes, we just pass control back to
    // an ordinary thread pool.
    std::cout << "---> Scheduling a task.\n";
    std::thread th(std::move(task));
    th.detach();
    return absl::OkStatus();
  }

  virtual ~MyCustomThreadPool() {}
};

}  // namespace

absl::Status Run() {
  std::shared_ptr<MyCustomThreadPool> thread_pool =
      std::make_shared<MyCustomThreadPool>();
  std::shared_ptr<Executor> executor =
      GENC_TRY(CreateDefaultExecutorWithThreadPool(thread_pool));
  Runner runner = GENC_TRY(Runner::Create(
      GENC_TRY(CreateModelInference("test_model")), executor));
  v0::Value arg;
  arg.set_str("Boo!");
  v0::Value output = GENC_TRY(runner.Run(arg));
  std::cout << "Result: " << output.DebugString() << "\n";
  return absl::OkStatus();
}
}  // namespace genc

int main(int argc, char* argv[]) {
  absl::Status status = genc::Run();
  if (!status.ok()) {
    std::cerr << "Error: " << status.message() << "\n";
  }
  return 0;
}
