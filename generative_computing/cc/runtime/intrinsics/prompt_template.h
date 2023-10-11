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

#ifndef GENERATIVE_COMPUTING_CC_RUNTIME_INTRINSICS_PROMPT_TEMPLATE_H_
#define GENERATIVE_COMPUTING_CC_RUNTIME_INTRINSICS_PROMPT_TEMPLATE_H_

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/cc/runtime/intrinsics/intrinsics.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

class PromptTemplate : public IntrinsicHandler {
 public:
  PromptTemplate() {}
  virtual ~PromptTemplate() {}

  absl::string_view uri() const final { return kPromptTemplate; }
  absl::Status CheckWellFormed(const v0::Intrinsic& intrinsic_pb) const final;
  absl::Status ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                           const v0::Value& arg,
                           const IntrinsicCallContext* call_context_or_nullptr,
                           v0::Value* result) const final;
};

}  // namespace intrinsics
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_INTRINSICS_PROMPT_TEMPLATE_H_
