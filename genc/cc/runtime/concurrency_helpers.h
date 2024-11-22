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
#include <memory>

#include "absl/base/thread_annotations.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/mutex.h"
#include "genc/cc/runtime/concurrency.h"

#ifndef GENC_CC_RUNTIME_CONCURRENCY_HELPERS_H_
#define GENC_CC_RUNTIME_CONCURRENCY_HELPERS_H_

namespace genc {

class CallbackTracker {
 public:
  CallbackTracker() {}

  int GetNewCallbackId() {
    absl::MutexLock l(&mutex_);
    return ++total_num_callbacks_scheduled_;
  }

  void RegisterCallback(int callback_id,
                        std::shared_ptr<WaitableInterface> waitable) {
    absl::MutexLock l(&mutex_);
    if (completed_early_.contains(callback_id)) {
      completed_early_.erase(callback_id);
    } else {
      callbacks_[callback_id] = waitable;
    }
  }

  void IgnoreCallback(int callback_id) {
    absl::MutexLock l(&mutex_);
    ++total_num_callbacks_completed_;
  }

  void RunCallback(std::function<void()> callback, int callback_id) {
    callback();
    absl::MutexLock l(&mutex_);
    if (callbacks_.contains(callback_id)) {
      callbacks_.erase(callback_id);
    } else {
      completed_early_.insert(callback_id);
    }
    ++total_num_callbacks_completed_;
  }

  absl::Status WaitUntilAllCompleted() {
    while (true) {
      int num_pending = -1;
      std::shared_ptr<WaitableInterface> waitable;
      {
        absl::MutexLock l(&mutex_);
        num_pending =
            total_num_callbacks_scheduled_ - total_num_callbacks_completed_;
        if (num_pending <= 0) return absl::OkStatus();
        if (!callbacks_.empty()) {
          waitable = callbacks_.begin()->second;
        }
      }
      VLOG(1) << "There are " << num_pending << " pending callbacks.";
      if (waitable != nullptr) {
        absl::Status status = waitable->Wait();
        if (!status.ok()) {
          LOG(ERROR) << "Wait status: " << status;
        }
      } else {
        return absl::Status(
            absl::StatusCode::kFailedPrecondition,
            "No waitable callbacks found; this potentially indicates "
            "that the wait attempt was made while activities were still "
            "being actively scheduled for execution.");
      }
    }
  }

 private:
  mutable absl::Mutex mutex_;
  int total_num_callbacks_scheduled_ ABSL_GUARDED_BY(mutex_) = 0;
  int total_num_callbacks_completed_ ABSL_GUARDED_BY(mutex_) = 0;
  absl::flat_hash_set<int> completed_early_ ABSL_GUARDED_BY(mutex_);
  absl::flat_hash_map<int, std::shared_ptr<WaitableInterface>> callbacks_
      ABSL_GUARDED_BY(mutex_);
};

class ConcurrencyManagerWithCallbackTracker
    : public ConcurrencyInterfaceWithWaitMethod {
 public:
  ConcurrencyManagerWithCallbackTracker()
      : callback_tracker_(std::make_shared<CallbackTracker>()) {}

  ~ConcurrencyManagerWithCallbackTracker() override {
    absl::Status status = WaitUntilAllCompleted();
    if (!status.ok()) {
      LOG(ERROR) << "Wait until all completed unsuccessful: " << status;
    }
  }

  absl::Status WaitUntilAllCompleted() override {
    return callback_tracker_->WaitUntilAllCompleted();
  }

 protected:
  virtual absl::StatusOr<std::shared_ptr<WaitableInterface>> ScheduleInternal(
      std::function<void()> callback) = 0;

  absl::StatusOr<std::shared_ptr<WaitableInterface>> Schedule(
      std::function<void()> callback) override {
    int callback_id = callback_tracker_->GetNewCallbackId();
    absl::StatusOr<std::shared_ptr<WaitableInterface>> waitable =
        ScheduleInternal([this, callback, callback_id]() {
          callback_tracker_->RunCallback(callback, callback_id);
        });
    if (waitable.ok()) {
      callback_tracker_->RegisterCallback(callback_id, waitable.value());
      return waitable.value();
    } else {
      callback_tracker_->IgnoreCallback(callback_id);
      return waitable.status();
    }
  }

 private:
  std::shared_ptr<CallbackTracker> callback_tracker_;
};

}  // namespace genc

#endif  // GENC_CC_RUNTIME_CONCURRENCY_HELPERS_H_
