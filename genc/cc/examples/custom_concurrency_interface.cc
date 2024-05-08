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
#include "absl/status/statusor.h"
#include "genc/cc/authoring/constructor.h"
#include "genc/cc/examples/executors/executor_stacks.h"
#include "genc/cc/runtime/concurrency.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/runner.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace {

class MyCustomConcurrencyInterface : public ConcurrencyInterface {
 public:
  ~MyCustomConcurrencyInterface() override {}

 protected:
  absl::StatusOr<std::shared_ptr<WaitableInterface>> Schedule(
      std::function<void()> callback) override {
    std::cout << "---> Schedule was called, executing the callback inline.\n";
    callback();
    return std::make_shared<Waitable>();
  }

 private:
  class Waitable : public WaitableInterface {
   public:
    Waitable() {}
    absl::Status Wait() override { return absl::OkStatus(); }
  };
};

}  // namespace

absl::Status Run() {
  std::shared_ptr<ConcurrencyInterface> concurrency_interface =
      std::make_shared<MyCustomConcurrencyInterface>();
  std::shared_ptr<Executor> executor = GENC_TRY(
      CreateDefaultExecutorWithConcurrencyInterface(concurrency_interface));
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
