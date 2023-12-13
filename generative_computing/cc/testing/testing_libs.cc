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

#include "generative_computing/cc/testing/testing_libs.h"

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "generative_computing/cc/runtime/status_macros.h"

namespace generative_computing {
namespace testing {

using ::generative_computing::v0::Value;

absl::StatusOr<Value> WrapFnNameAroundValue(std::string fn_name,
                                            const Value& value) {
  std::string formatted_value_str = GENC_TRY(FormatValueAsString(value));
  std::string formatted_str =
      absl::StrFormat("%s(%s)", fn_name, formatted_value_str);
  Value output_value;
  output_value.set_str(formatted_str);
  return output_value;
}

// Returns the content of value, if value contains struct, returns the content
// of the struct separated by ','. used for testing purpose.
absl::StatusOr<std::string> FormatValueAsString(const Value& value) {
  switch (value.value_case()) {
    case Value::kStr:
      return value.str();
    case Value::kBoolean:
      return value.boolean() ? "true" : "false";
    case Value::kInt32:
      return std::to_string(value.int_32());
    case Value::kFloat32:
      return std::to_string(value.float_32());
    case Value::kMedia:
      return value.media();
    case Value::kStruct: {
      std::string formatted_str = "";
      for (const auto& element : value.struct_().element()) {
        std::string element_str = GENC_TRY(FormatValueAsString(element));
        if (formatted_str.empty()) {
          absl::StrAppend(&formatted_str, element_str);
        } else {
          absl::StrAppend(&formatted_str, ",", element_str);
        }
      }
      return formatted_str;
    }
    default:
      return absl::UnimplementedError(
          absl::StrCat("Unable to format, unsupported value type [",
                       value.value_case(), "]"));
  }
}
}  // namespace testing
}  // namespace generative_computing
