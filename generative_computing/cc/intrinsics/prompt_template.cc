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

#include "generative_computing/cc/intrinsics/prompt_template.h"

#include <string>

#include "absl/status/status.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/proto/v0/computation.pb.h"
#include "re2/re2.h"

namespace generative_computing {
namespace intrinsics {

absl::Status PromptTemplate::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (intrinsic_pb.static_parameter_size() != 1) {
    return absl::InvalidArgumentError("Wrong number of arguments.");
  }
  return absl::OkStatus();
}

absl::Status PromptTemplate::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                         const v0::Value& arg,
                                         v0::Value* result) const {
  // TODO(b/295041950): Add support for handling prompt templates with
  // multiple arguments, not just a single string (so the argument can
  // in general be a struct with multiple values, not just one).
  const absl::string_view template_string(
      intrinsic_pb.static_parameter(0).value().str());
  absl::string_view input(template_string);
  std::string parameter_re = "(\\{[a-zA-Z0-9_]*\\})";
  std::string parameter;
  if (!RE2::FindAndConsume(&input, parameter_re, &parameter)) {
    return absl::InvalidArgumentError("No parameter found in the template.");
  }
  std::string other_parameter;
  while (RE2::FindAndConsume(&input, parameter_re, &other_parameter)) {
    if (other_parameter != parameter) {
      return absl::InvalidArgumentError("Multiple parameters not supported.");
    }
  }
  result->mutable_str()->assign(
      absl::StrReplaceAll(template_string, {{parameter, arg.str()}}));
  return absl::OkStatus();
}

}  // namespace intrinsics
}  // namespace generative_computing
