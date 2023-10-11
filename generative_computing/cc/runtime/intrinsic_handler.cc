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

#include "generative_computing/cc/runtime/intrinsic_handler.h"

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/synchronization/mutex.h"
#include "generative_computing/cc/runtime/status_macros.h"

namespace generative_computing {

IntrinsicHandlerSet::~IntrinsicHandlerSet() {
  absl::MutexLock l(&handlers_lock_);
  for (auto& pair : handlers_) {
    delete pair.second;
  }
}

void IntrinsicHandlerSet::AddHandler(const IntrinsicHandler* handler) {
  absl::MutexLock l(&handlers_lock_);
  handlers_[handler->uri()] = handler;
}

absl::Status IntrinsicHandlerSet::ExecuteCall(
    const v0::Intrinsic& intrinsic_pb, const v0::Value& arg,
    const IntrinsicCallContext* call_context_or_nullptr, v0::Value* result) {
  const IntrinsicHandler* handler;
  {
    absl::ReaderMutexLock l(&handlers_lock_);
    handler = handlers_[intrinsic_pb.uri()];
  }
  GENC_TRY(handler->CheckWellFormed(intrinsic_pb));
  return handler->ExecuteCall(intrinsic_pb, arg, call_context_or_nullptr,
                              result);
}

}  // namespace generative_computing
