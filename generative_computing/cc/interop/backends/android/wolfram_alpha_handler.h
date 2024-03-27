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

#ifndef GENERATIVE_COMPUTING_CC_INTEROP_BACKENDS_ANDROID_WOLFRAM_ALPHA_HANDLER_H_
#define GENERATIVE_COMPUTING_CC_INTEROP_BACKENDS_ANDROID_WOLFRAM_ALPHA_HANDLER_H_

#include <jni.h>

#include <memory>

#include "absl/status/status.h"
#include "generative_computing/cc/intrinsics/intrinsic_uris.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

class WolframAlphaHandler : public InlineIntrinsicHandlerBase {
 public:
  WolframAlphaHandler(JavaVM* jvm, jobject wolfram_alpha_client)
      : InlineIntrinsicHandlerBase(kWolframAlpha) {
    jvm_ = jvm;
    wolfram_alpha_client_ = wolfram_alpha_client;
  }
  virtual ~WolframAlphaHandler() {}

  absl::Status CheckWellFormed(const v0::Intrinsic& intrinsic_pb) const final;

  absl::Status ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                           const v0::Value& arg, v0::Value* result) const final;

 private:
  absl::StatusOr<v0::Value> call(const v0::Value& func,
                                 const v0::Value& arg) const;

  // not owned.
  JavaVM* jvm_;
  jobject wolfram_alpha_client_;
};
}  // namespace intrinsics
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_INTEROP_BACKENDS_ANDROID_WOLFRAM_ALPHA_HANDLER_H_
