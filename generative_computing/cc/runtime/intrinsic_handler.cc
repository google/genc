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
#include "absl/strings/str_cat.h"
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

absl::Status IntrinsicHandlerSet::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                              const v0::Value& arg,
                                              v0::Value* result) const {
  const IntrinsicHandler* handler = nullptr;
  {
    absl::ReaderMutexLock l(&handlers_lock_);
    auto it = handlers_.find(intrinsic_pb.uri());
    if (it != handlers_.end()) {
      handler = it->second;
    }
  }
  if (handler == nullptr) {
    return absl::NotFoundError(
        absl::StrCat("Could not find a handler for: ", intrinsic_pb.uri()));
  }
  if (handler->uri() != intrinsic_pb.uri()) {
    return absl::InternalError(
        absl::StrCat("Wrong handler URI registered for: ", intrinsic_pb.uri()));
  }
  if (handler->interface_type() != IntrinsicHandler::InterfaceType::INLINE) {
    return absl::InternalError(absl::StrCat(
        "Wrong handler interface type registered for: ", intrinsic_pb.uri()));
  }
  GENC_TRY(handler->CheckWellFormed(intrinsic_pb));
  const InlineIntrinsicHandlerInterface* const interface =
      dynamic_cast<const InlineIntrinsicHandlerInterface*>(handler);
  if (interface == nullptr) {
    return absl::InternalError(absl::StrCat(
        "Could not access the handler interface for: ", intrinsic_pb.uri()));
  }
  return interface->ExecuteCall(intrinsic_pb, arg, result);
}

}  // namespace generative_computing
