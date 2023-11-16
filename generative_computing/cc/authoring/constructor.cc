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
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/intrinsics/intrinsic_uris.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {

v0::Value CreateLabeledValue(std::string label, const v0::Value& value) {
  v0::Value labeled_value;
  labeled_value = value;
  labeled_value.set_label(label);
  return labeled_value;
}

}  // namespace

absl::StatusOr<v0::Value> SmartChain::Build() {
  // Enable BreakableChain.
  if (num_iteration_ > 1) {
    return CreateLoopChainCombo(num_iteration_, chained_ops_);
  }
  return CreateBasicChain(chained_ops_);
}

absl::StatusOr<v0::Value> CreateRepeat(int num_steps, v0::Value body_fn) {
  v0::Value repeat_pb;
  v0::Intrinsic* const intrinsic_pb = repeat_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kRepeat));
  v0::Struct* args =
      intrinsic_pb->mutable_static_parameter()->mutable_struct_();
  v0::Value* step_pb = args->add_element();
  step_pb->set_label("num_steps");
  step_pb->set_int_32(num_steps);
  *args->add_element() = CreateLabeledValue("body_fn", body_fn);
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

absl::StatusOr<v0::Value> CreateCustomFunction(absl::string_view fn_uri) {
  v0::Value fn_pb;
  v0::Intrinsic* const intrinsic_pb = fn_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kCustomFunction));
  intrinsic_pb->mutable_static_parameter()->set_str(std::string(fn_uri));
  return fn_pb;
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

absl::StatusOr<v0::Value> CreateCall(v0::Value fn, v0::Value arg) {
  v0::Value value_pb;
  v0::Call* call_pb = value_pb.mutable_call();
  *call_pb->mutable_function() = fn;
  *call_pb->mutable_argument() = arg;
  return value_pb;
}

absl::StatusOr<v0::Value> CreatePromptTemplate(absl::string_view template_str) {
  v0::Value value_pb;
  v0::Intrinsic* const intrinsic_pb = value_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kPromptTemplate));
  intrinsic_pb->mutable_static_parameter()->set_str(std::string(template_str));
  return value_pb;
}

absl::StatusOr<v0::Value> CreateRegexPartialMatch(
    absl::string_view pattern_str) {
  v0::Value value_pb;
  v0::Intrinsic* const intrinsic_pb = value_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kRegexPartialMatch));
  intrinsic_pb->mutable_static_parameter()->set_str(std::string(pattern_str));
  return value_pb;
}

absl::StatusOr<v0::Value> CreateLogicalNot() {
  v0::Value value_pb;
  v0::Intrinsic* const intrinsic_pb = value_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kLogicalNot));
  return value_pb;
}

absl::StatusOr<v0::Value> CreateWhile(v0::Value condition_fn,
                                      v0::Value body_fn) {
  v0::Value while_pb;
  v0::Intrinsic* const intrinsic_pb = while_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kWhile));
  v0::Struct* args =
      intrinsic_pb->mutable_static_parameter()->mutable_struct_();
  *args->add_element() = CreateLabeledValue("condition_fn", condition_fn);
  *args->add_element() = CreateLabeledValue("body_fn", body_fn);
  return while_pb;
}

absl::StatusOr<v0::Value> CreateLoopChainCombo(
    int num_steps, std::vector<v0::Value> body_fns) {
  v0::Value combo_pb;
  v0::Intrinsic* const intrinsic_pb = combo_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kLoopChainCombo));
  v0::Struct* args =
      intrinsic_pb->mutable_static_parameter()->mutable_struct_();
  v0::Value* steps_pb = args->add_element();
  steps_pb->set_label("num_steps");
  steps_pb->set_int_32(num_steps);

  for (int i = 0; i < body_fns.size(); i++) {
    *args->add_element() =
        CreateLabeledValue(absl::StrFormat("fn_%d", i), body_fns[i]);
  }
  return combo_pb;
}

absl::StatusOr<v0::Value> CreateBreakableChain(
    std::vector<v0::Value> fns_list) {
  v0::Value chain_pb;
  v0::Intrinsic* const intrinsic_pb = chain_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kBreakableChain));
  v0::Struct* args =
      intrinsic_pb->mutable_static_parameter()->mutable_struct_();
  for (int i = 0; i < fns_list.size(); i++) {
    *args->add_element() =
        CreateLabeledValue(absl::StrFormat("fn_%d", i), fns_list[i]);
  }
  return chain_pb;
}

absl::StatusOr<v0::Value> CreateBasicChain(
    std::vector<v0::Value> function_list) {
  v0::Value chain_pb;
  v0::Intrinsic* const intrinsic_pb = chain_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kBasicChain));
  v0::Struct* args =
      intrinsic_pb->mutable_static_parameter()->mutable_struct_();
  for (int i = 0; i < function_list.size(); i++) {
    *args->add_element() =
        CreateLabeledValue(absl::StrFormat("fn_%d", i), function_list[i]);
  }
  return chain_pb;
}

absl::StatusOr<v0::Value> CreateParallelMap(v0::Value map_fn) {
  v0::Value map_pb;
  v0::Intrinsic* const intrinsic_pb = map_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kParallelMap));
  *intrinsic_pb->mutable_static_parameter() = map_fn;
  return map_pb;
}

absl::StatusOr<v0::Value> CreateLogger() {
  v0::Value logger_pb;
  v0::Intrinsic* const intrinsic_pb = logger_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kLogger));
  return logger_pb;
}

absl::StatusOr<v0::Value> CreateStruct(std::vector<v0::Value> value_list) {
  v0::Value value_pb;
  auto mutable_element = value_pb.mutable_struct_()->mutable_element();
  for (const auto& value : value_list) {
    *mutable_element->Add() = value;
  }
  return value_pb;
}

absl::StatusOr<v0::Value> CreateSelection(v0::Value source, int index) {
  v0::Value value_pb;
  v0::Selection* selection_pb = value_pb.mutable_selection();
  selection_pb->set_index(index);
  *selection_pb->mutable_source() = source;
  return value_pb;
}

absl::StatusOr<v0::Value> CreateFallback(std::vector<v0::Value> function_list) {
  v0::Value fallback_pb;
  v0::Intrinsic* const intrinsic_pb = fallback_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kFallback));
  v0::Struct* args =
      intrinsic_pb->mutable_static_parameter()->mutable_struct_();
  for (const auto& fn_pb : function_list) {
    *args->add_element() = CreateLabeledValue("candidate_fn", fn_pb);
  }
  return fallback_pb;
}

absl::StatusOr<v0::Value> CreateConditional(v0::Value condition,
                                            v0::Value positive_branch,
                                            v0::Value negative_branch) {
  v0::Value conditional_pb;
  v0::Intrinsic* const intrinsic_pb = conditional_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kConditional));
  v0::Struct* args =
      intrinsic_pb->mutable_static_parameter()->mutable_struct_();
  *args->add_element() = CreateLabeledValue("then", positive_branch);
  *args->add_element() = CreateLabeledValue("else", negative_branch);
  return CreateCall(conditional_pb, condition);
}

}  // namespace generative_computing
