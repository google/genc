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

#ifndef GENERATIVE_COMPUTING_CC_TYPES_INFERENCE_H_
#define GENERATIVE_COMPUTING_CC_TYPES_INFERENCE_H_

#include "absl/status/status.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

struct InferenceOptions {};

absl::Status InferTypes(v0::Value* value_pb,
                        const InferenceOptions* options_or_null = nullptr);

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_TYPES_INFERENCE_H_
