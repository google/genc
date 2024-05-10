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

#include "genc/cc/authoring/tracing_context.h"

#include <optional>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "genc/cc/authoring/constructor.h"
#include "genc/cc/modules/retrieval/bi_map.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
absl::StatusOr<v0::Value> TracingContext::Call(
    const v0::Value& portable_ir, const std::vector<v0::Value>& args) {
  if (args.empty()) {
    return absl::InvalidArgumentError("At least one argument is required.");
  }

  std::vector<v0::Value> processed_args;
  processed_args.reserve(args.size());
  for (const auto& arg : args) {
    processed_args.push_back(GENC_TRY(ReplaceWithReference(arg)));
  }
  v0::Value input_val = ToValue(processed_args);

  v0::Value processed_func = GENC_TRY(ReplaceWithReference(portable_ir));
  return CreateCall(processed_func, input_val);
}

absl::StatusOr<v0::Value> TracingContext::ReplaceWithReference(
    const v0::Value& ir) {
  if (ir.has_reference()) {
    return ir;
  }

  std::string serialized_ir = ir.SerializeAsString();
  std::optional<std::string> optional_name =
      name_to_ir_.FindByValue(serialized_ir);
  if (!optional_name.has_value()) {
    optional_name = absl::StrCat("v_", name_to_ir_.size());
    name_to_ir_.Insert(optional_name.value(), serialized_ir);
  }
  return CreateReference(optional_name.value());
}
}  // namespace genc
