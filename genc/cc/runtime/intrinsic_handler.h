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

#ifndef GENC_CC_RUNTIME_INTRINSIC_HANDLER_H_
#define GENC_CC_RUNTIME_INTRINSIC_HANDLER_H_

#include <memory>
#include <optional>
#include <string>

#include "absl/base/const_init.h"
#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"
#include "genc/cc/runtime/concurrency.h"
#include "genc/cc/runtime/executor.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

// An interface implemented by handlers of the `INLINE` interface type.
class InlineIntrinsicHandlerInterface {
 public:
  class Context {
   public:
    virtual std::shared_ptr<ConcurrencyInterface> concurrency_interface()
        const = 0;
    virtual ~Context() {}
  };

  virtual absl::Status ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                   const v0::Value& arg, v0::Value* result,
                                   Context* context) const = 0;

  virtual ~InlineIntrinsicHandlerInterface() {}
};

// An interface implemented by handlers of the `CONTROL_FLOW` interface type.
class ControlFlowIntrinsicHandlerInterface {
 public:
  class Value {
   public:
    virtual ~Value() {}
  };

  typedef std::shared_ptr<Value> ValueRef;

  class Context : public ExecutorInterface<ValueRef, ValueRef> {
   public:
    virtual std::shared_ptr<ConcurrencyInterface> concurrency_interface()
        const = 0;
    virtual ~Context() {};
  };

  virtual absl::StatusOr<ValueRef> ExecuteCall(
      const v0::Intrinsic& intrinsic_pb, std::optional<ValueRef> arg,
      Context* context) const = 0;

  virtual ~ControlFlowIntrinsicHandlerInterface() {}
};

// Defines a handler for a single intrinsic, to be used by executors in the
// runtime stack.
class IntrinsicHandler {
 public:
  enum InterfaceType { INLINE, CONTROL_FLOW };

  static absl::StatusOr<const InlineIntrinsicHandlerInterface*>
  GetInlineInterface(const IntrinsicHandler* handler);

  static absl::StatusOr<const ControlFlowIntrinsicHandlerInterface*>
  GetControlFlowInterface(const IntrinsicHandler* handler);

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
class IntrinsicHandlerSet {
 public:
  IntrinsicHandlerSet() : handlers_lock_(absl::kConstInit), handlers_() {}

  // Adds handler to the set. Transfers ownership. The hander set will own the
  // handler, and will delete it upon destruction.
  void AddHandler(const IntrinsicHandler* handler);

  // Returns the specified handler.
  absl::StatusOr<const IntrinsicHandler*> GetHandler(
      absl::string_view uri) const;

  ~IntrinsicHandlerSet();

 private:
  mutable absl::Mutex handlers_lock_;
  absl::flat_hash_map<std::string, const IntrinsicHandler*> handlers_;
};

}  // namespace genc

#endif  // GENC_CC_RUNTIME_INTRINSIC_HANDLER_H_
