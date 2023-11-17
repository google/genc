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

#ifndef GENERATIVE_COMPUTING_CC_AUTHORING_CONSTRUCTOR_H_
#define GENERATIVE_COMPUTING_CC_AUTHORING_CONSTRUCTOR_H_

#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

// Given arg_name & computation body create a Lambda that applies a computation
// to the provided argument.
absl::StatusOr<v0::Value> CreateLambda(absl::string_view arg_name,
                                       v0::Value body);

// Constructs a function call.
absl::StatusOr<v0::Value> CreateCall(v0::Value fn, v0::Value arg);

// Given arg_name returns a Reference argument. Useful for parameterizing the
// computation.
absl::StatusOr<v0::Value> CreateReference(absl::string_view arg_name);

// Returns a repeat proto which will repeat body_fn for num_steps, sequentially,
// the output of the current step is the input to next iteration.
absl::StatusOr<v0::Value> CreateRepeat(int num_steps, v0::Value body_fn);

// Returns a model inference proto with the given model URI.
absl::StatusOr<v0::Value> CreateModelInference(absl::string_view model_uri);

// Returns a custom function proto with the given fn URI.
absl::StatusOr<v0::Value> CreateCustomFunction(absl::string_view fn_uri);

// Populate the computation.proto in `intrinsics` to represent a model
// inference call to a model `model_uri`.
void SetModelInference(v0::Value& computation, absl::string_view model_uri);

// Creates a logical negation computation.
absl::StatusOr<v0::Value> CreateLogicalNot();

// Creates a prompt template computation with the given template string.
absl::StatusOr<v0::Value> CreatePromptTemplate(absl::string_view template_str);

// Creates a partial regex matcher with the given template string.
absl::StatusOr<v0::Value> CreateRegexPartialMatch(
    absl::string_view pattern_str);

// Creates a while loop with the given condition_fn, body_fn.
absl::StatusOr<v0::Value> CreateWhile(v0::Value condition_fn,
                                      v0::Value body_fn);

// Creates a parallel map that applies map_fn to a all input values.
absl::StatusOr<v0::Value> CreateParallelMap(v0::Value map_fn);

// Creates a for loop with the given num_steps, and a sequence of body_fns,
// which will be executed sequentially each iteration, if any the function
// inside body_fns is a conditional, and list evaluate to be true, it'll exit
// the for loop.
absl::StatusOr<v0::Value> CreateLoopChainCombo(int num_steps,
                                               std::vector<v0::Value> body_fns);

// Given a list of functions [f, g, ...] create a chain g(f(...)).
absl::StatusOr<v0::Value> CreateBasicChain(
    std::vector<v0::Value> function_list);

// Creates a Logger, it takes an input logs it and returns the original input.
absl::StatusOr<v0::Value> CreateLogger();

// Given a list of functions [f, g, ...] create a chain g(f(...)). Compared to
// CreateBasicChain, this chain can contain break point as part of the chain.
absl::StatusOr<v0::Value> CreateBreakableChain(std::vector<v0::Value> fns_list);

// Constructs a struct from named values
absl::StatusOr<v0::Value> CreateStruct(std::vector<v0::Value> value_list);

// Constructs a selection to pick the i-th element from a source.struct_.
absl::StatusOr<v0::Value> CreateSelection(v0::Value source, int index);

// Creates a fallback expression from a given list of functions. The first
// successful one is the result; if failed, keep going down the list.
absl::StatusOr<v0::Value> CreateFallback(std::vector<v0::Value> function_list);

// Creates a conditional expression. condition evaluates to a boolean, if true,
// positive_branch will be executed, else negative_branch.
absl::StatusOr<v0::Value> CreateConditional(v0::Value condition,
                                            v0::Value positive_branch,
                                            v0::Value negative_branch);
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_AUTHORING_CONSTRUCTOR_H_
