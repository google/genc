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

#ifndef GENERATIVE_COMPUTING_CC_RUNTIME_INTRINSIC_HANDLER_H_
#define GENERATIVE_COMPUTING_CC_RUNTIME_INTRINSIC_HANDLER_H_

#include <string>

#include "absl/base/const_init.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

// Represents the execution context for an intrinsic call.
class IntrinsicCallContext {};

// Defines a handler for a single intrinsic, to be used by executors in the
// runtime stack.
class IntrinsicHandler {
 public:
  // Returns the URI of the intrinsic implemented by this handler.
  virtual absl::string_view uri() const = 0;

  // Checks that the definition of the intrinsic is well-formed, e.g., that
  // the static parameters are defined correctly.
  virtual absl::Status CheckWellFormed(
      const v0::Intrinsic& intrinsic_pb) const = 0;

  // Implements the call of this intrinsic. The supplied call context is owned
  // by the caller. It may be null, or its methods may be unimplemented if the
  // call context is not present durign the call.
  virtual absl::Status ExecuteCall(
      const v0::Intrinsic& intrinsic_pb, const v0::Value& arg,
      const IntrinsicCallContext* call_context_or_nullptr,
      v0::Value* result) const = 0;

  virtual ~IntrinsicHandler() {}
};

// Represents a set of intrinsic handlers.
class IntrinsicHandlerSet {
 public:
  IntrinsicHandlerSet() : handlers_lock_(absl::kConstInit), handlers_() {}

  // Adds handler to the set. Transfers ownership. The hander set will own the
  // handler, and will delete it upon destruction.
  void AddHandler(const IntrinsicHandler* handler);

  // Routes the `ExecuteCall` to the appropriate handler in the set.
  absl::Status ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                           const v0::Value& arg,
                           const IntrinsicCallContext* call_context_or_nullptr,
                           v0::Value* result);

  ~IntrinsicHandlerSet();

 private:
  absl::Mutex handlers_lock_;
  absl::flat_hash_map<std::string, const IntrinsicHandler*> handlers_;
};

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_INTRINSIC_HANDLER_H_
