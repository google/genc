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

#ifndef GENC_CC_BASE_CONTEXT_H_
#define GENC_CC_BASE_CONTEXT_H_

#include <memory>
#include <vector>

#include "absl/base/thread_annotations.h"
#include "absl/status/status.h"
#include "absl/synchronization/mutex.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

// An abstract interface to be implemented by all types of contexts.
// For instance, authoring context to trace computations.
class Context {
 public:
  virtual ~Context() = default;

  // Synchronously process a call to portable_ir in this context.
  virtual v0::Value Call(const v0::Value& portable_ir,
                         const std::vector<v0::Value>& args) = 0;
};

// The shared stack of contexts.
class ContextStack {
 public:
  explicit ContextStack() = default;
  explicit ContextStack(std::shared_ptr<Context> ctx) : default_context_(ctx) {}

  // Makes `ctx` the default context, replacing whatever was current default.
  void SetDefaultContext(std::shared_ptr<Context> ctx);

  // Appends `ctx` as the top of the context stack.
  void AppendNestedContext(std::shared_ptr<Context> ctx);

  // Removes `ctx` from the top of the context stack.
  absl::Status RemoveNestedContext(std::shared_ptr<Context> ctx);

  // Returns the context at the top of the stack (or default one if none).
  std::shared_ptr<Context> CurrentContext() const;

 private:
  mutable absl::Mutex mutex_;
  std::shared_ptr<Context> default_context_ ABSL_GUARDED_BY(mutex_);
  std::vector<std::shared_ptr<Context>> nested_contexts_
      ABSL_GUARDED_BY(mutex_);
};

std::shared_ptr<ContextStack> GetContextStack();

}  // namespace genc

#endif  // GENC_CC_BASE_CONTEXT_H_
