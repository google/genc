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

#ifndef GENERATIVE_COMPUTING_CC_INTRINSICS_H_
#define GENERATIVE_COMPUTING_CC_INTRINSICS_H_

#include "absl/strings/string_view.h"

namespace generative_computing {
namespace intrinsics {

// Conditional expression that dynamically chooses among two alternatives to be
// lazily evaluated, based on a Boolean input argument. The alternatives must
// have matching types, but otherwise can be anything, from simple scalar values
// to functions. Neither of the anternatives is proactively evaluated, parsed,
// or initialized until after evaluating the input condition, and only one of
// the two is lazily evaluated based on the input to the conditional.
// Takes two static parameters named "then" and "else" that represent the two
// alternatives to select among.
// Takes exactly one dynamic parameter, which must evaluate to a Boolean result.
// Based on the materialized value of that result, evaluates and returns either
// "then" or "else" as the output of the conditional.
inline constexpr absl::string_view kConditional = "conditional";

// Performs a fallback across a list of candidate computations that are to be
// lazily attempted in the order listed (i.e., they are not evaluated, parsed,
// or initialized until it's their turn to attempt in the sequence).
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

// Represents regex partial match function.
// Takes one static parameter named "regex_pattern" of a string type,
// representing the pattern to match.
// Takes one dynamic string parameter, which is the input string matched against
// the supplied pattern.
// Returns a boolean scalar. The returned values is True if partial match found
// a match, False otherwise.
inline constexpr absl::string_view kRegexPartialMatch = "regex_partial_match";

}  // namespace intrinsics
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_INTRINSICS_H_
