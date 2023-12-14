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

#ifndef GENERATIVE_COMPUTING_CC_TESTING_TESTING_LIBS_H_
#define GENERATIVE_COMPUTING_CC_TESTING_TESTING_LIBS_H_
// Common libs used for simplify testing code or running mock examples.
#include <string>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace testing {

// Returns a str in format of fn_name(formatted_value).
absl::StatusOr<std::string> WrapFnNameAroundValue(
    std::string fn_name, const generative_computing::v0::Value& value);

// Returns a Value wraps the input str.
generative_computing::v0::Value StrAsValue(std::string str);

// Returns a Value wraps the input media bytes.
generative_computing::v0::Value MediaAsValue(absl::string_view media);

// Returns the content of value, if value contains struct, returns the content
// of the struct separated by ','. used for testing purpose.
absl::StatusOr<std::string> FormatValueAsString(
    const generative_computing::v0::Value& value);

}  // namespace testing
}  // namespace generative_computing
#endif  // GENERATIVE_COMPUTING_CC_TESTING_TESTING_LIBS_H_
