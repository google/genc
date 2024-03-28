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

#include "genc/cc/intrinsics/inja_template.h"

#include <string>

#include "absl/status/status.h"
#include "absl/strings/substitute.h"
#include "genc/cc/modules/templates/inja_status_or.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"
#include <nlohmann/json.hpp>

namespace genc {
namespace intrinsics {

absl::Status InjaTemplate::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_str()) {
    return absl::InvalidArgumentError("Expect template as str, got none.");
  }
  return absl::OkStatus();
}

absl::Status InjaTemplate::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                       const v0::Value& arg,
                                       v0::Value* result) const {
  const std::string template_str(intrinsic_pb.static_parameter().str());
  auto parsed_json = nlohmann::json::parse(arg.str(), /*cb=*/nullptr,
                                           /*allow_exceptions=*/false);
  if (parsed_json.is_discarded()) {
    return absl::InternalError(absl::Substitute(
        "Failed parsing json input to InjaTemplate: $0", arg.DebugString()));
  }

  // TODO(b/315223622): externalize env, and enable the custom callback fns.
  inja_status_or::Environment env;
  std::string result_str = GENC_TRY(env.render(template_str, parsed_json));
  result->set_str(result_str);
  return absl::OkStatus();
}

}  // namespace intrinsics
}  // namespace genc
