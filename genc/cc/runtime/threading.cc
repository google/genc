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

#include <functional>
#include <future>  // NOLINT
#include <memory>
#include <thread>  // NOLINT
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "genc/cc/runtime/concurrency.h"

namespace genc {
namespace {

class ThreadBasedConcurrencyManager : public ConcurrencyInterface {
 public:
  ~ThreadBasedConcurrencyManager() override {}

 protected:
  absl::StatusOr<std::shared_ptr<WaitableInterface>> Schedule(
      std::function<void()> callback) override {
    std::packaged_task<void()> task([callback]() { callback(); });
    auto future_ptr = std::shared_future<void>(task.get_future());
    std::thread th(std::move(task));
    th.detach();
    return std::make_shared<Waitable>(future_ptr);
  }

 private:
  class Waitable : public WaitableInterface {
   public:
    Waitable(std::shared_future<void> future) : future_(std::move(future)) {}
    absl::Status Wait() override {
      future_.wait();
      return absl::OkStatus();
    }

   private:
    std::shared_future<void> future_;
  };
};

}  // namespace

std::shared_ptr<ConcurrencyInterface> CreateThreadBasedConcurrencyManager() {
  return std::make_shared<ThreadBasedConcurrencyManager>();
}

}  // namespace genc
