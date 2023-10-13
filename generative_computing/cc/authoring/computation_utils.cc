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

#include "generative_computing/cc/authoring/computation_utils.h"

#include <string>

#include "absl/strings/string_view.h"
#include "generative_computing/cc/intrinsics/intrinsic_uris.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

void SetModelInference(v0::Computation* computation,
                       absl::string_view model_uri) {
  v0::Intrinsic* const intrinsic_pb = computation->mutable_intrinsic();
  intrinsic_pb->set_uri(
      std::string(::generative_computing::intrinsics::kModelInference));
  v0::Intrinsic::StaticParameter* const static_parameter_pb =
      intrinsic_pb->add_static_parameter();
  static_parameter_pb->set_name("model_uri");
  static_parameter_pb->mutable_value()->set_str(std::string(model_uri));
}

}  // namespace generative_computing
