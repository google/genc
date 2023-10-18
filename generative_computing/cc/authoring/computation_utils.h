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

#ifndef GENERATIVE_COMPUTING_CC_RUNTIME_TEST_UTILS_H_
#define GENERATIVE_COMPUTING_CC_RUNTIME_TEST_UTILS_H_

#include "absl/strings/string_view.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

// Populate the computation.proto in `intrinsics` to represent a model
// inference call to a model `model_uri`.
void SetModelInference(v0::Value& computation, absl::string_view model_uri);

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_TEST_UTILS_H_
