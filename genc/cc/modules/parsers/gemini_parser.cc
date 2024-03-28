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

#include "genc/cc/modules/parsers/gemini_parser.h"

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/substitute.h"
#include "genc/cc/intrinsics/custom_function.h"
#include "genc/cc/modules/templates/inja_status_or.h"
#include "genc/cc/runtime/status_macros.h"
#include <nlohmann/json.hpp>

namespace genc {

absl::StatusOr<v0::Value> GeminiParser::GetTopCandidateAsText(v0::Value input) {
  auto parsed_json = nlohmann::json::parse(input.str(), /*cb=*/nullptr,
                                           /*allow_exceptions=*/false);
  if (parsed_json.is_discarded()) {
    return absl::InternalError(absl::Substitute(
        "Failed parsing json output from Gemini: $0", input.DebugString()));
  }

  std::string extract_first_candidate_as_text =
      "{% if candidates %}{% for p in candidates.0.content.parts "
      "%}{{p.text}}{% endfor %}{%   endif %}";

  inja_status_or::Environment env;

  v0::Value result;
  std::string result_str =
      GENC_TRY(env.render(extract_first_candidate_as_text, parsed_json));
  result.set_str(result_str);
  return result;
}

absl::StatusOr<v0::Value> GeminiParser::WrapTextAsInputJson(v0::Value input) {
  std::string json_request = absl::Substitute(
      R"pb(
        {
          "contents":
          [ {
            "parts":
            [ { "text": "$0" }]
          }]
        }
      )pb",
      input.str());
  v0::Value result;
  result.set_str(json_request);
  return result;
}

absl::Status GeminiParser::SetCustomFunctions(
    intrinsics::CustomFunction::FunctionMap& fn_map) {
  fn_map["/gemini_parser/get_top_candidate_as_text"] =
      [](const v0::Value& arg) {
        return GeminiParser::GetTopCandidateAsText(arg);
      };

  fn_map["/gemini_parser/wrap_text_as_input_json"] = [](const v0::Value& arg) {
    return GeminiParser::WrapTextAsInputJson(arg);
  };

  return absl::OkStatus();
}
}  // namespace genc
