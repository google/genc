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

// Defines a handler for a single intrinsic, to be used by executors in the
// runtime stack.
class IntrinsicHandler {
 public:
  enum InterfaceType { INLINE, CONTROL_FLOW };

  // Returns the URI of the intrinsic implemented by this handler.
  virtual absl::string_view uri() const = 0;

  // Returns the type of the interface implemented by this handler.
  virtual InterfaceType interface_type() const = 0;

  // Checks that the definition of the intrinsic is well-formed, e.g., that
  // the static parameters are defined correctly.
  virtual absl::Status CheckWellFormed(
      const v0::Intrinsic& intrinsic_pb) const = 0;

  virtual ~IntrinsicHandler() {}
};

// An interface implemented by handlers of the `INLINE` interface type.
class InlineIntrinsicHandlerInterface {
 public:
  virtual absl::Status ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                   const v0::Value& arg,
                                   v0::Value* result) const = 0;

  virtual ~InlineIntrinsicHandlerInterface() {}
};

// Represents the execution context for a control flow intrinsic.
class ControlFlowIntrinsicCallContext {
 public:
  virtual ~ControlFlowIntrinsicCallContext() {}
};

// An interface implemented by handlers of the `CONTROL_FLOW` interface type.
class ControlFlowIntrinsicHandlerInterface {
 public:
  virtual ~ControlFlowIntrinsicHandlerInterface() {}
};

// Base class for inline intrinsic handlers.
class InlineIntrinsicHandlerBase : public IntrinsicHandler,
                                   public InlineIntrinsicHandlerInterface {
 public:
  InlineIntrinsicHandlerBase(absl::string_view uri) : uri_(uri) {}

  absl::string_view uri() const { return uri_; }
  InterfaceType interface_type() const { return InterfaceType::INLINE; }

 private:
  const std::string uri_;
};

// Base class for control flow intrinsic handlers.
class ControlFlowIntrinsicHandlerBase
    : public IntrinsicHandler,
      public ControlFlowIntrinsicHandlerInterface {
 public:
  ControlFlowIntrinsicHandlerBase(absl::string_view uri) : uri_(uri) {}

  absl::string_view uri() const { return uri_; }
  InterfaceType interface_type() const { return InterfaceType::CONTROL_FLOW; }

 private:
  const std::string uri_;
};

// Represents a set of intrinsic handlers.
class IntrinsicHandlerSet : public InlineIntrinsicHandlerInterface,
                            public ControlFlowIntrinsicHandlerInterface {
 public:
  IntrinsicHandlerSet() : handlers_lock_(absl::kConstInit), handlers_() {}

  // Adds handler to the set. Transfers ownership. The hander set will own the
  // handler, and will delete it upon destruction.
  void AddHandler(const IntrinsicHandler* handler);

  // Routes the `ExecuteCall` to the appropriate handler in the set.
  absl::Status ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                           const v0::Value& arg, v0::Value* result) const;

  ~IntrinsicHandlerSet();

 private:
  mutable absl::Mutex handlers_lock_;
  absl::flat_hash_map<std::string, const IntrinsicHandler*> handlers_;
};

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_INTRINSIC_HANDLER_H_
