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

#ifndef GENERATIVE_COMPUTING_CC_RUNTIME_THREADING_H_
#define GENERATIVE_COMPUTING_CC_RUNTIME_THREADING_H_

#include <functional>
#include <future> // NOLINT
#include <memory>
#include <thread> // NOLINT
#include <type_traits>
#include <utility>

#include "absl/status/status.h"

namespace generative_computing {

// Placeholder for a thread pool implementation, to be pulled in when necessary.
class ThreadPool {
 public:
  virtual absl::Status Schedule(std::function<void()> task) = 0;
  virtual ~ThreadPool() {}
};

// Runs the provided provided no-arg function on another thread, returning a
// future to the result. Each task will be scheduled on a newly created thread.
// TODO(b/295015950): Bring in the thread pool support here if truly necessary.
template <typename Func,
          typename ReturnValue = typename std::result_of_t<Func()>>
std::shared_future<ReturnValue> ThreadRun(Func lambda,
                                          ThreadPool* thread_pool = nullptr) {
  using TaskT = std::packaged_task<ReturnValue()>;
  TaskT task(std::move(lambda));
  auto future_ptr = std::shared_future<ReturnValue>(task.get_future());
  if (thread_pool != nullptr) {
    thread_pool->Schedule(
        [t = std::make_shared<TaskT>(std::move(task))]() { (*t)(); });
  } else {
    std::thread th(std::move(task));
    th.detach();
  }
  return future_ptr;
}

// Awaits the result of a ValueFuture, usually a future returning a
// StatusOr<ExecutorValue>. Returns the resulting status or value wrapped again
// as a StatusOr.
template <typename ValueFuture>
auto Wait(const ValueFuture& future) {
  future.wait();
  const auto& result = future.get();
  using StatusOrValue = typename std::remove_reference<decltype(result)>::type;
  if (!result.ok()) {
    return StatusOrValue(result.status());
  }
  return StatusOrValue(result.value());
}

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_THREADING_H_
