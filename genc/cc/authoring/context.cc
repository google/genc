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

#include "genc/cc/authoring/context.h"

#include <memory>

#include "absl/status/status.h"

namespace genc {

void ContextStack::SetDefaultContext(std::shared_ptr<Context> ctx) {
  default_context_ = ctx;
}

void ContextStack::AppendNestedContext(std::shared_ptr<Context> ctx) {
  nested_contexts_.push_back(ctx);
}

absl::Status ContextStack::RemoveNestedContext(std::shared_ptr<Context> ctx) {
  if (nested_contexts_.empty()) {
    return absl::FailedPreconditionError("No nested context on the stack.");
  }
  if (nested_contexts_.back() != ctx) {
    return absl::FailedPreconditionError(
        "Can only remove the top context from the stack.");
  }
  nested_contexts_.pop_back();
  return absl::OkStatus();
}

std::shared_ptr<Context> ContextStack::CurrentContext() const {
  if (!nested_contexts_.empty()) {
    return nested_contexts_.back();
  }
  return default_context_;
}
}  // namespace genc
