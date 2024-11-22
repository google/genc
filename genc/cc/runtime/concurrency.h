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

#ifndef GENC_CC_RUNTIME_CONCURRENCY_H_
#define GENC_CC_RUNTIME_CONCURRENCY_H_

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace genc {

template <typename ReturnValue>
class FutureInterface {
 public:
  virtual absl::StatusOr<ReturnValue> Get() = 0;
  virtual ~FutureInterface() {}
};

class WaitableInterface {
 public:
  virtual absl::Status Wait() = 0;
  virtual ~WaitableInterface() {}
};

class ConcurrencyInterface {
 public:
  template <typename Func,
            typename ReturnValue = typename std::result_of_t<Func()>>
  std::shared_ptr<FutureInterface<ReturnValue>> RunAsync(Func lambda) {
    std::shared_ptr<Task<Func>> task =
        std::make_shared<Task<Func>>(std::move(lambda));
    task->SetWaitable(Schedule([task]() { task->Run(); }));
    return task;
  }

  virtual ~ConcurrencyInterface() {}

 protected:
  virtual absl::StatusOr<std::shared_ptr<WaitableInterface>> Schedule(
      std::function<void()> callback) = 0;

 private:
  template <typename Func,
            typename ReturnValue = typename std::result_of_t<Func()>>
  class Task : public FutureInterface<ReturnValue> {
   public:
    Task(Func func) : func_(std::move(func)) {}
    virtual ~Task() {}
    void Run() {
      if (std::is_void_v<ReturnValue>) {
        std::move(func_)();
      } else {
        result_ = std::move(func_)();
      }
    }
    absl::StatusOr<ReturnValue> Get() override {
      if (!waitable_.ok()) {
        return waitable_.status();
      }
      absl::Status status = waitable_.value()->Wait();
      if (!status.ok()) {
        return status;
      }
      return result_;
    }
    void SetWaitable(
        absl::StatusOr<std::shared_ptr<WaitableInterface>> waitable) {
      waitable_ = std::move(waitable);
    }

   private:
    Func func_;
    ReturnValue result_;
    absl::StatusOr<std::shared_ptr<WaitableInterface>> waitable_;
  };
};

class ConcurrencyInterfaceWithWaitMethod : public ConcurrencyInterface {
 public:
  // Wait until al processing that has been initiated so far is completed.
  // This is a blocking call.
  // REQUIRES: All callbacks-scheduling calls that may have been invoked in
  // parallel must have completed. An error may be returned otherwise,
  // indicating that the wait was unsuccessful, and the wait call should be
  // invoked again. Similarly, i any processing is triggered after this call,
  // you may want to call this method again. Multiple calls are permitted.
  virtual absl::Status WaitUntilAllCompleted() = 0;

  virtual ~ConcurrencyInterfaceWithWaitMethod() {}
};

}  // namespace genc

#endif  // GENC_CC_RUNTIME_CONCURRENCY_H_
