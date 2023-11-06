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
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/string_view.h"
#include "generative_computing/proto/v0/computation.pb.h"
#include "re2/re2.h"

namespace generative_computing {
namespace intrinsics {
constexpr absl::string_view kParameterRe = "(\\{[a-zA-Z0-9_]*\\})";

absl::Status PromptTemplate::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_str()) {
    return absl::InvalidArgumentError(
        "Expect prompt_template as str, got none.");
  }
  return absl::OkStatus();
}

absl::Status PromptTemplate::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                         const v0::Value& arg,
                                         v0::Value* result) const {
  const absl::string_view template_string(
      intrinsic_pb.static_parameter().str());
  absl::string_view input(template_string);
  std::string parameter;
  // Extract template placeholders.
  absl::flat_hash_set<std::string> parameters_set;
  while (RE2::FindAndConsume(&input, kParameterRe, &parameter)) {
    parameters_set.insert(parameter);
  }

  std::vector<std::pair<std::string, std::string>> replacements = {};
  if (parameters_set.size() == 1) {
    // Handle univariate template.
    if (!arg.has_str()) {
      return absl::InvalidArgumentError(
          "Expect input to PromptTemplate to have str value, got none.");
    }
    replacements.emplace_back(parameter, arg.str());
  } else {
    // Handle multivariate template.
    for (const auto& arg : arg.struct_().element()) {
      replacements.emplace_back(absl::StrFormat("{%s}", arg.name()),
                                arg.value().str());
    }
  }

  result->set_str(absl::StrReplaceAll(template_string, replacements));
  return absl::OkStatus();
}

}  // namespace intrinsics
}  // namespace generative_computing
