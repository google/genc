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

#include "genc/cc/authoring/constructor.h"

#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "genc/cc/intrinsics/intrinsic_uris.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

namespace {

v0::Value CreateLabeledValue(std::string label, const v0::Value& value) {
  v0::Value labeled_value;
  labeled_value = value;
  labeled_value.set_label(label);
  return labeled_value;
}

}  // namespace

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

absl::StatusOr<v0::Value> CreateRestModelConfig(std::string endpoint,
                                                std::string api_key) {
  v0::Value model_config_pb;
  model_config_pb.set_label("model_config");
  if (api_key.empty()) {
    model_config_pb.set_str(endpoint);
    return model_config_pb;
  }

  v0::Struct* args = model_config_pb.mutable_struct_();

  v0::Value* endpoint_pb = args->add_element();
  endpoint_pb->set_label("endpoint");
  endpoint_pb->set_str(endpoint);

  v0::Value* api_key_pb = args->add_element();
  api_key_pb->set_label("api_key");
  api_key_pb->set_str(api_key);

  return model_config_pb;
}

absl::StatusOr<v0::Value> CreateRestModelConfigWithJsonRequestTemplate(
    std::string endpoint,
    std::string api_key,
    std::string json_request_template) {
  v0::Value model_config_pb = GENC_TRY(
      CreateRestModelConfig(endpoint, api_key));
  v0::Value* template_pb = model_config_pb.mutable_struct_()->add_element();
  template_pb->set_label("json_request_template");
  template_pb->set_str(json_request_template);
  return model_config_pb;
}

absl::StatusOr<v0::Value> CreateLlamaCppConfig(std::string model_path,
                                               int num_threads,
                                               int max_tokens) {
  v0::Value model_config_pb;
  model_config_pb.set_label("model_config");
  v0::Struct* args = model_config_pb.mutable_struct_();

  v0::Value* model_path_pb = args->add_element();
  model_path_pb->set_label("model_path");
  model_path_pb->set_str(model_path);

  v0::Value* num_threads_pb = args->add_element();
  num_threads_pb->set_label("num_threads");
  num_threads_pb->set_int_32(num_threads);

  v0::Value* max_tokens_pb = args->add_element();
  max_tokens_pb->set_label("max_tokens");
  max_tokens_pb->set_int_32(max_tokens);

  return model_config_pb;
}

// TODO(b/325090417): merge into CreateModelInference with nullable config.
absl::StatusOr<v0::Value> CreateModelInferenceWithConfig(
    absl::string_view model_uri, v0::Value model_config) {
  v0::Value model_pb;
  v0::Intrinsic* const intrinsic_pb = model_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kModelInferenceWithConfig));
  v0::Struct* args =
      intrinsic_pb->mutable_static_parameter()->mutable_struct_();
  v0::Value* model_uri_pb = args->add_element();
  model_uri_pb->set_label("model_uri");
  model_uri_pb->set_str(std::string(model_uri));

  v0::Value* model_config_pb = args->add_element();
  model_config_pb->set_label("model_config");
  if (model_config.has_struct_()) {
    *model_config_pb->mutable_struct_() = model_config.struct_();
  } else if (model_config.has_str()) {
    model_config_pb->set_str(model_config.str());
  }

  return model_pb;
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

absl::StatusOr<v0::Value> CreatePromptTemplateWithParameters(
    absl::string_view template_str,
    std::vector<absl::string_view> parameter_list) {
  if (parameter_list.size() < 2) {
    return absl::InvalidArgumentError(absl::StrCat(
        "There must be at least 2 parameters, got: ", parameter_list.size()));
  }
  v0::Value value_pb;
  v0::Intrinsic* const intrinsic_pb = value_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kPromptTemplateWithParameters));
  v0::Value template_element;
  template_element.set_str(std::string(template_str));
  v0::Value params_element;
  for (const absl::string_view& param_name : parameter_list) {
    params_element.mutable_struct_()->mutable_element()->Add()->set_str(
        param_name);
  }
  std::vector<v0::Value> elements;
  elements.push_back(template_element);
  elements.push_back(params_element);
  *intrinsic_pb->mutable_static_parameter() = GENC_TRY(CreateStruct(elements));
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

absl::StatusOr<v0::Value> CreateRepeatedConditionalChain(
    int num_steps, std::vector<v0::Value> body_fns) {
  v0::Value combo_pb;
  v0::Intrinsic* const intrinsic_pb = combo_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kRepeatedConditionalChain));
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

absl::StatusOr<v0::Value> CreateSerialChain(
    std::vector<v0::Value> function_list) {
  v0::Value chain_pb;
  v0::Intrinsic* const intrinsic_pb = chain_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kSerialChain));
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

absl::StatusOr<v0::Value> CreateNamedValue(
    absl::string_view label, v0::Value unlabeled_value) {
  v0::Value value_pb = unlabeled_value;
  value_pb.set_label(label);
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

absl::StatusOr<v0::Value> CreateLambdaForConditional(
    v0::Value condition, v0::Value positive_branch, v0::Value negative_branch) {
  v0::Value arg_ref = GENC_TRY(CreateReference("arg"));

  return CreateLambda("arg",
                      GENC_TRY(CreateConditional(
                          GENC_TRY(CreateCall(condition, arg_ref)),
                          GENC_TRY(CreateCall(positive_branch, arg_ref)),
                          GENC_TRY(CreateCall(negative_branch, arg_ref)))));
}

absl::StatusOr<v0::Value> CreateInjaTemplate(absl::string_view template_str) {
  v0::Value value_pb;
  v0::Intrinsic* const intrinsic_pb = value_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kInjaTemplate));
  intrinsic_pb->mutable_static_parameter()->set_str(std::string(template_str));
  return value_pb;
}

absl::StatusOr<v0::Value> CreateRestCall(absl::string_view uri,
                                         absl::string_view api_key,
                                         absl::string_view method) {
  v0::Value rest_call_pb;
  v0::Intrinsic* const intrinsic_pb = rest_call_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kRestCall));
  v0::Struct* args =
      intrinsic_pb->mutable_static_parameter()->mutable_struct_();
  v0::Value* method_pb = args->add_element();
  method_pb->set_label("method");
  method_pb->set_str(std::string(method));

  v0::Value* uri_pb = args->add_element();
  uri_pb->set_label("uri");
  uri_pb->set_str(std::string(uri));

  v0::Value* api_key_pb = args->add_element();
  api_key_pb->set_label("api_key");
  api_key_pb->set_str(std::string(api_key));

  return rest_call_pb;
}

absl::StatusOr<v0::Value> CreateWolframAlpha(absl::string_view appid) {
  v0::Value wolfram_alpha_pb;
  v0::Intrinsic* const intrinsic_pb = wolfram_alpha_pb.mutable_intrinsic();
  intrinsic_pb->set_uri(std::string(intrinsics::kWolframAlpha));
  intrinsic_pb->mutable_static_parameter()->set_str(std::string(appid));
  intrinsic_pb->mutable_static_parameter()->set_label("appid");
  return wolfram_alpha_pb;
}

}  // namespace genc
