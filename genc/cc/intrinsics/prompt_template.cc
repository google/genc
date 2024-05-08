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

#include "genc/cc/intrinsics/prompt_template.h"

#include <string>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/string_view.h"
#include "genc/proto/v0/computation.pb.h"
#include "re2/re2.h"

namespace genc {
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
                                         v0::Value* result,
                                         Context* context) const {
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
      replacements.emplace_back(absl::StrFormat("{%s}", arg.label()),
                                arg.str());
    }
  }

  result->set_str(absl::StrReplaceAll(template_string, replacements));
  return absl::OkStatus();
}

absl::Status PromptTemplateWithParameters::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_struct_()) {
    return absl::InvalidArgumentError("Expect prompt_template as struct.");
  }
  if (intrinsic_pb.static_parameter().struct_().element_size() != 2) {
    return absl::InvalidArgumentError(
        "Expect prompt_template as struct with two elements.");
  }
  if (!intrinsic_pb.static_parameter().struct_().element(0).has_str()) {
    return absl::InvalidArgumentError(
        "Expect prompt_template as struct with str as the first element.");
  }
  if (!intrinsic_pb.static_parameter().struct_().element(1).has_struct_()) {
    return absl::InvalidArgumentError(
        "Expect prompt_template as struct with struct as the second element.");
  }
  auto params = intrinsic_pb.static_parameter().struct_().element(1).struct_();
  if (params.element_size() < 2) {
    return absl::InvalidArgumentError(
        "Expect prompt_template as struct with a 2-or-more-element struct "
        "as the second element.");
  }
  for (int index = 0; index < params.element_size(); ++index) {
    if (!params.element(index).has_str()) {
      return absl::InvalidArgumentError(
          "Expected strings in the parameter list.");
    }
  }
  return absl::OkStatus();
}

absl::Status PromptTemplateWithParameters::ExecuteCall(
    const v0::Intrinsic& intrinsic_pb, const v0::Value& arg, v0::Value* result,
    Context* context) const {
  const absl::string_view template_string(
      intrinsic_pb.static_parameter().struct_().element(0).str());
  if (template_string.empty()) {
    return absl::InvalidArgumentError("The template string is empty.");
  }
  if (!arg.has_struct_()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "The argument is not a struct: ", arg.DebugString()));
  }
  auto params = intrinsic_pb.static_parameter().struct_().element(1).struct_();
  if (params.element_size() != arg.struct_().element_size()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Mismatching the number of elements in the argument: ",
        arg.struct_().element_size(),
        " as compated to ",
        params.element_size(),
        " in the template: ",
        intrinsic_pb.static_parameter().struct_().DebugString()));
  }
  absl::string_view input(template_string);
  std::string parameter;
  // Extract template placeholders.
  absl::flat_hash_set<std::string> parameters_set;
  while (RE2::FindAndConsume(&input, kParameterRe, &parameter)) {
    parameters_set.insert(parameter);
  }
  std::vector<std::pair<std::string, std::string>> replacements = {};
  for (int index = 0; index < params.element_size(); ++index) {
    const v0::Value& arg_element = arg.struct_().element(index);
    if (!arg_element.has_str()) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Expected strings in the argument, got ", arg_element.DebugString()));
    }
    const v0::Value& param_element = params.element(index);
    if (!param_element.has_str()) {
      return absl::InternalError("Non-string parameter name.");
    }
    replacements.emplace_back(
        absl::StrFormat("{%s}", param_element.str()), arg_element.str());
  }
  result->set_str(absl::StrReplaceAll(template_string, replacements));
  return absl::OkStatus();
}

}  // namespace intrinsics
}  // namespace genc
