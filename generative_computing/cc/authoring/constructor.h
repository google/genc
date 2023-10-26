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

// Given a list of functions [f, g, ...] create a chain f(g(...)).
absl::StatusOr<v0::Value> CreateChain(std::vector<v0::Value> function_list);

// Returns a repeat proto which will repeat body_fn for num_steps, sequentially,
// the output of the current step is the input to next iteration.
absl::StatusOr<v0::Value> CreateRepeat(int num_steps,
                                       absl::string_view body_fn_name);

// Returns a model inference proto with the given model URI.
absl::StatusOr<v0::Value> CreateModelInference(absl::string_view model_uri);

// Populate the computation.proto in `intrinsics` to represent a model
// inference call to a model `model_uri`.
void SetModelInference(v0::Value& computation, absl::string_view model_uri);

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_AUTHORING_CONSTRUCTOR_H_
