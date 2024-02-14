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

#include "generative_computing/cc/intrinsics/model_inference_with_config.h"

#include <string>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

absl::Status ModelInferenceWithConfig::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_struct_()) {
    return absl::InvalidArgumentError("Expected struct_ to be set, got none.");
  }
  if (!intrinsic_pb.static_parameter().struct_().element(0).has_str()) {
    return absl::InvalidArgumentError(
        "Expected model_uri to be set, got none.");
  }
  return absl::OkStatus();
}

absl::Status ModelInferenceWithConfig::ExecuteCall(
    const v0::Intrinsic& intrinsic_pb,
    const v0::Value& arg,
    v0::Value* result) const {
  const std::string model_uri(
      intrinsic_pb.static_parameter().struct_().element(0).str());
  if (model_uri == "test_model") {
    result->set_str(
        absl::StrCat("This is an output from a test model in response to \"",
                     arg.str(), "\"."));
    return absl::OkStatus();
  }
  if (inference_map_.contains(model_uri)) {
    *result = GENC_TRY(inference_map_.at(model_uri)(intrinsic_pb, arg));
    return absl::OkStatus();
  }

  // TODO(b/295260921): Based on a prefix of the URI embedded in the `Model`,
  // route calls to an appropriate child executor or child component that
  // specializes in handling calls for the corresponding class of models.
  // The set of supported models and child executors is something that should
  // come in as a parameter (since we want it to be a point of flexibility we
  // offer to whoever is handling runtime deployment and configuration).
  // In particular, we should be able to effectively route the calls to either
  // an on-device or a Cloud executor (and in general, there could be multiple
  // of each supported here within the same runtime stack).
  return absl::UnimplementedError(
      absl::StrCat("Unsupported model: ", model_uri));
}

}  // namespace intrinsics
}  // namespace generative_computing
