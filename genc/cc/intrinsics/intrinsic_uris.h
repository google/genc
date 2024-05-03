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

#ifndef GENC_CC_INTRINSIC_URIS_H_
#define GENC_CC_INTRINSIC_URIS_H_

#include "absl/strings/string_view.h"

namespace genc {
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

// Delegates processing to a named runtime environment (e.g., from a device
// to remotely execute within a Cloud backend). The processing being delegated
// can be an arbitrary computation (e.g., a chain or a workflow) as long as
// all intrinsics within that computation are supported by the remote backends.
// The static parameter is a struct, the first element of which is the URI of
// the runtime environment to delegate to, and the second of which is the
// computation to delegate to run in that environment.
// The dynamic parameter is the argument for that computation (to be uploaded
// to the environment in which it is going to be executed).
inline constexpr absl::string_view kDelegate = "delegate";

// Performs a fallback across a list of candidate computations that are to be
// lazily attempted in the order listed (i.e., they are not evaluated, parsed,
// or initialized until it's their turn to attempt in the sequence).
// Takes any number of function-typed static parameters named "candidate_fn"
// that represent functions to attempt to invoke in the order listed.
// Takes exactly one dynamic parameter, which serves as the input to each of
// the candidate functions (all of which must have matching type signatures).
inline constexpr absl::string_view kFallback = "fallback";

// Represents a logical not.
// Takes zero static parameters.
// Takes one dynamic parameter containing the boolean to be flipped.
inline constexpr absl::string_view kLogicalNot = "logical_not";

// Calls a model for inference.
// Takes one static parameter "model_uri" of a string type.
// Takes one dynamic string parameter, which serves as the input to the model.
inline constexpr absl::string_view kModelInference = "model_inference";

// Calls a model for inference.
// Takes one static struct_ parameter containing 'model_uri' of a string type
// and 'model_config' of a string or struct_ type.
// Takes one dynamic string parameter, which serves as the input to the model.
inline constexpr absl::string_view kModelInferenceWithConfig =
        "model_inference_with_config";

// Calls a user defined custom function.
// Takes one static parameter "fn_uri" of a string type.
// Takes one dynamic value parameter, which serves as the input to the function.
inline constexpr absl::string_view kCustomFunction = "custom_function";

// Constructs a prompt.
// Takes one static parameter "template_string" of a string type.
// Takes (currently just) one dynamic string parameter, which serves as the
// parameter for the template).
inline constexpr absl::string_view kPromptTemplate = "prompt_template";

// Constructs a prompt wiht a given list of parameters.
// Takes a struct as a static parameter with two elements, the first being the
// "template_string" of a string type, and the second being another struct that
// enumerates parameters in the order in which they will be supplied in the
// calls at runtime. The dynamic parameter is a struct without labels in which
// the order of elements must match the order of the parameters specified here.
inline constexpr absl::string_view kPromptTemplateWithParameters =
        "prompt_template_with_parameters";

// Represents regex partial match function.
// Takes one static parameter named "regex_pattern" of a string type,
// representing the pattern to match.
// Takes one dynamic string parameter, which is the input string matched against
// the supplied pattern.
// Returns a boolean scalar. The returned values is True if partial match found
// a match, False otherwise.
inline constexpr absl::string_view kRegexPartialMatch = "regex_partial_match";

// Represents a loop that repeats its logic n times sequentially .
// Takes one static parameter contains body_fn and num_steps.
// Takes one dynamic Value parameter, which serves as the input to the loop.
inline constexpr absl::string_view kRepeat = "repeat";

// Represents a while loop that repeats until the condition evaluates to false.
// The condition is evaluated first, including before the first iteration.
// Takes one static struct_ parameter containing 'condition_fn' and 'body_fn'.
// Takes one dynamic Value parameter, which serves as the input to the loop.
inline constexpr absl::string_view kWhile = "while";

// Represents a for loop that repeats itself for given number of steps, and can
// flexibally break during execution. Takes one static parameter contains
// num_steps, and chain of body_fns, fn1, fn2..., which will be executed
// sequentially, if any of those function evaluates to be true, the for loop
// will break. It takes one dynamic Value parameter, which serves as the input
// to the loop.
inline constexpr absl::string_view kRepeatedConditionalChain =
    "repeated_conditional_chain";

// Represents a custom chain, a chain of fns, h, g, f... will be executed
// sequentially.
// If g returns non-bool, then g(f(x)) is being fed further as input into
// h(...). If g returns true, then f(x) is returned by the chain. If g returns
// false, then f(x) is being fed further as input into h(...).
//  Chain by nature are compositional, one chain can contain other chains,
inline constexpr absl::string_view kBreakableChain = "breakable_chain";

// Represents a serial chain, a chain of fns, h, g, f... will be executed
// sequentially. h(g(f(x))).
// Compared to BreakableChain, this chain doesn't break, it simply executes
// functions one by one.
inline constexpr absl::string_view kSerialChain = "serial_chain";

// Represents a logger, prints the value and returns the original value.
inline constexpr absl::string_view kLogger = "logger";

// Represents a map_fn that applies the the fn to nested Values in parallel.
inline constexpr absl::string_view kParallelMap = "parallel_map";

// Represents an inja template, which is a more powerful template engine
// compared to PromptTemplate. Input json & template can utilize the JSON;
// outputs formatted string.
inline constexpr absl::string_view kInjaTemplate = "inja_template";

// Represents a REST call.
inline constexpr absl::string_view kRestCall = "rest_call";

// Represents a WolframAlpha Call.
inline constexpr absl::string_view kWolframAlpha = "wolfram_alpha";

// Represents a call to a Confidential Computing backend. The static parameter
// is a struct that includes the computation as the first element, and backend
// config struct with labeled values as the second element. The backend config,
// at minimum, contains the server address, labeled "server_address", and the
// image digest, labeled "image_digest", both strings. The dynamic parameter is
// the parameter to feed to the confidential computation to be executed.
inline constexpr absl::string_view kConfidentialComputation =
    "confidential_computation";

}  // namespace intrinsics
}  // namespace genc

#endif  // GENC_CC_INTRINSIC_URIS_H_
