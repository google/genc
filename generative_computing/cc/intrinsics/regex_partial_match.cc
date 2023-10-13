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

#include "generative_computing/cc/intrinsics/regex_partial_match.h"

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/proto/v0/computation.pb.h"
#include "re2/re2.h"

namespace generative_computing {
namespace intrinsics {

absl::Status RegexPartialMatch::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (intrinsic_pb.static_parameter_size() != 1) {
    return absl::InvalidArgumentError("Wrong number of arguments.");
  }
  return absl::OkStatus();
}

absl::Status RegexPartialMatch::ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                            const v0::Value& arg,
                                            v0::Value* result) const {
  result->set_boolean(RE2::PartialMatch(
      arg.str(), intrinsic_pb.static_parameter(0).value().str()));
  return absl::OkStatus();
}

}  // namespace intrinsics
}  // namespace generative_computing
