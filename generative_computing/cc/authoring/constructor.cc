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

#include "generative_computing/cc/authoring/constructor.h"

#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/intrinsics/intrinsic_uris.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

absl::StatusOr<v0::Value> CreateRepeat(int num_steps,
                                       absl::string_view body_fn_name) {
  v0::Value repeat_pb;
  v0::Intrinsic* const intrinsic_pb = repeat_pb.mutable_intrinsic();
  intrinsic_pb->set_uri("repeat");
  v0::Struct* args =
      intrinsic_pb->mutable_static_parameter()->mutable_struct_();
  v0::NamedValue* step_pb = args->add_element();
  step_pb->set_name("num_steps");
  step_pb->mutable_value()->set_int_32(num_steps);
  v0::NamedValue* body_pb = args->add_element();
  body_pb->set_name("body_fn");
  *body_pb->mutable_value() = GENC_TRY(CreateModelInference(body_fn_name));
  return repeat_pb;
}

absl::StatusOr<v0::Value> CreateModelInference(absl::string_view model_uri) {
  v0::Value model_pb;
  SetModelInference(model_pb, model_uri);
  return model_pb;
}

void SetModelInference(v0::Value& computation, absl::string_view model_uri) {
  v0::Intrinsic* const intrinsic_pb = computation.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kModelInference));
  intrinsic_pb->mutable_static_parameter()->set_str(std::string(model_uri));
}

absl::StatusOr<v0::Value> CreateLambda(absl::string_view arg_name,
                                       v0::Value body) {
  v0::Value value_pb;
  v0::Lambda* lambda_pb = value_pb.mutable_lambda();
  lambda_pb->set_parameter_name(std::string(arg_name));
  (*lambda_pb->mutable_result()) = body;
  return value_pb;
}

absl::StatusOr<v0::Value> CreateReference(absl::string_view arg_name) {
  v0::Value value_pb;
  v0::Reference* reference_pb = value_pb.mutable_reference();
  reference_pb->set_name(std::string(arg_name));
  return value_pb;
}

absl::StatusOr<v0::Value> CreateChain(std::vector<v0::Value> function_list) {
  std::string parameter_name = "arg";
  v0::Value arg_pb = GENC_TRY(CreateReference(parameter_name));

  for (std::vector<v0::Value>::reverse_iterator it = function_list.rbegin();
       it != function_list.rend(); ++it) {
    arg_pb = GENC_TRY(CreateCall(*it, arg_pb));
  }
  return CreateLambda(parameter_name, arg_pb);
}

absl::StatusOr<v0::Value> CreateCall(v0::Value fn, v0::Value arg) {
  v0::Value value_pb;
  v0::Call* call_pb = value_pb.mutable_call();
  *call_pb->mutable_function() = fn;
  *call_pb->mutable_argument() = arg;
  return value_pb;
}

}  // namespace generative_computing
