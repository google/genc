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

#ifndef GENERATIVE_COMPUTING_CC_RUNTIME_INTRINSICS_H_
#define GENERATIVE_COMPUTING_CC_RUNTIME_INTRINSICS_H_

#include "absl/strings/string_view.h"

namespace generative_computing {
namespace intrinsics {

// Performs a fallback across a list of candidate computations.
// Takes any number of function-typed static parameters named "candidate_fn"
// that represent functions to attempt to invoke in the order listed.
// Takes exactly one dynamic parameter, which serves as the input to each of
// the candidate functions (all of which must have matching type signatures).
inline constexpr absl::string_view kFallback = "fallback";

// Calls a model for inference.
// Takes one static parameter "model_uri" of a string type.
// Takes one dynamic string parameter, which serves as the input to the model.
inline constexpr absl::string_view kModelInference = "model_inference";

// Constructs a prompt.
// Takes one static parameter "template_string" of a string type.
// Takes (currently just) one dynamic string parameter, which serves as the
// parameter for the template).
inline constexpr absl::string_view kPromptTemplate = "prompt_template";

}  // namespace intrinsics
}  // namespace generative_computing


#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_INTRINSICS_H_
