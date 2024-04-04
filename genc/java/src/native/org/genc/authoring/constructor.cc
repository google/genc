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

#include <jni.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "genc/cc/authoring/constructor.h"
#include "genc/java/src/jni/jni_utils.h"

namespace genc {

namespace {

// Converts a C++ proto to Java byte array. Returns nullptr if an error occurs.
template <typename T>
absl::StatusOr<jbyteArray> GetJbyteArrayFromProto(JNIEnv* env,
                                                  const T& message) {
  if (env == nullptr) {
    return absl::InvalidArgumentError("JNI environment is null.");
  }
  const int32_t length = message.ByteSizeLong();
  jbyteArray output = env->NewByteArray(length);
  if (output == nullptr) {
    return absl::InternalError("Failed to allocate buffer.");
  }
  jbyte* bytes = env->GetByteArrayElements(output, nullptr);
  if (bytes == nullptr) {
    return absl::InternalError("Failed to get byte array elements.");
  }
  bool status = message.SerializeToArray(static_cast<void*>(bytes), length);
  env->ReleaseByteArrayElements(output, bytes, 0);
  if (!status) {
    env->DeleteLocalRef(output);
    return absl::InternalError("Proto serialization to array failed.");
  }
  return output;
}

// Converts a proto serialized as Java byte array to C++ proto of type `T`.
// Returns an error if conversion fails.
template <typename T>
absl::StatusOr<T> ParseProtoFromByteArray(JNIEnv* env, jbyteArray jbyte_array) {
  if (env == nullptr) {
    return absl::InvalidArgumentError("JNI environment is null.");
  }
  size_t size = env->GetArrayLength(jbyte_array);
  jbyte* bytes = env->GetByteArrayElements(jbyte_array, nullptr);
  if (bytes == nullptr) {
    return absl::InternalError("Failed to get byte array elements.");
  }
  T proto;
  if (!proto.ParseFromArray(reinterpret_cast<const char*>(bytes), size)) {
    return absl::InvalidArgumentError(
        "Failed to parse proto from Java byte array");
  }
  env->ReleaseByteArrayElements(jbyte_array, bytes, 0);
  return proto;
}

// Returns value in StatusOr if present.
// If a value is not present, logs an error and returns the default_value.
// One can use the default_value to return nullptr or another default value as
// seen fit for specific usage.
template <typename T>
T GetValueOrLogError(const absl::StatusOr<T>& status_or_value,
                     T default_value) {
  if (!status_or_value.ok()) {
    LOG(ERROR) << "An error occurred: " << status_or_value.status();
    return default_value;
  }
  return *status_or_value;
}

std::vector<v0::Value> GetValueVectorFromJavaList(JNIEnv* env, jobject list) {
  // Create a std::vector to hold the converted elements
  std::vector<v0::Value> vec;

  // Get the class of java.util.List
  jclass listClass = env->GetObjectClass(list);
  jmethodID sizeMethodId = env->GetMethodID(listClass, "size", "()I");
  if (sizeMethodId == nullptr) {
    return vec;
  }

  // Get the get(int index) method of java.util.List
  jmethodID getMethodId =
      env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
  if (getMethodId == nullptr) {
    return vec;
  }

  // Get the size of the list
  jint size = env->CallIntMethod(list, sizeMethodId);

  // Populate the vector with elements from the Java List
  for (int i = 0; i < size; ++i) {
    // Call the get(int index) method to get the element at index i
    jbyteArray element = reinterpret_cast<jbyteArray>(
        env->CallObjectMethod(list, getMethodId, i));

    absl::StatusOr<v0::Value> element_proto =
        ParseProtoFromByteArray<v0::Value>(env, element);
    if (!element_proto.ok()) {
      LOG(ERROR) << "Failed to parse Value proto: " << element_proto.status();
      vec.clear();
      return vec;
    }

    // Add the converted element to the vector
    vec.push_back(element_proto.value());
  }
  return vec;
}

}  // namespace

jbyteArray GC_create_model_inference(JNIEnv* env, jstring model_uri) {
  if (model_uri == nullptr) {
    LOG(ERROR) << "model_uri is null.";
    return nullptr;
  }
  std::string model_uri_str = GetString(env, model_uri);
  absl::string_view uri(model_uri_str);
  absl::StatusOr<v0::Value> model_inference = CreateModelInference(uri);
  if (!model_inference.ok()) {
    LOG(ERROR) << "Failed to create model inference: "
               << model_inference.status();
    return nullptr;
  }
  const v0::Value computation_proto = model_inference.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_model_inference_with_config(JNIEnv* env, jstring model_uri,
                                                 jbyteArray config) {
  if (model_uri == nullptr) {
    LOG(ERROR) << "model_uri is null.";
    return nullptr;
  }
  std::string model_uri_str = GetString(env, model_uri);
  absl::string_view uri(model_uri_str);
  absl::StatusOr<v0::Value> config_proto =
      ParseProtoFromByteArray<v0::Value>(env, config);
  if (!config_proto.ok()) {
    LOG(ERROR) << "Failed to parse config proto: " << config_proto.status();
    return nullptr;
  }
  absl::StatusOr<v0::Value> model_inference =
      CreateModelInferenceWithConfig(uri, config_proto.value());
  if (!model_inference.ok()) {
    LOG(ERROR) << "Failed to create model inference with config: "
               << model_inference.status();
    return nullptr;
  }
  const v0::Value computation_proto = model_inference.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_prompt_template(JNIEnv* env, jstring template_str) {
  std::string template_string = GetString(env, template_str);
  absl::string_view template_str_view(template_string);
  absl::StatusOr<v0::Value> prompt_template =
      CreatePromptTemplate(template_str_view);
  if (!prompt_template.ok()) {
    LOG(ERROR) << "Failed to create prompt template: "
               << prompt_template.status();
    return nullptr;
  }
  const v0::Value computation_proto = prompt_template.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_fallback(JNIEnv* env, jobject list) {
  std::vector<v0::Value> vec = GetValueVectorFromJavaList(env, list);
  if (vec.empty()) {
    return nullptr;
  }
  absl::StatusOr<v0::Value> fallback = CreateFallback(vec);
  if (!fallback.ok()) {
    LOG(ERROR) << "Failed to create fallback computation proto: "
               << fallback.status();
    return nullptr;
  }
  const v0::Value computation_proto = fallback.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_conditional(JNIEnv* env, jbyteArray condition,
                                 jbyteArray positive_branch,
                                 jbyteArray negative_branch) {
  absl::StatusOr<v0::Value> condition_proto =
      ParseProtoFromByteArray<v0::Value>(env, condition);
  absl::StatusOr<v0::Value> positive_branch_proto =
      ParseProtoFromByteArray<v0::Value>(env, positive_branch);
  absl::StatusOr<v0::Value> negative_branch_proto =
      ParseProtoFromByteArray<v0::Value>(env, negative_branch);
  if (!condition_proto.ok() || !positive_branch_proto.ok() ||
      !negative_branch_proto.ok()) {
    LOG(ERROR) << "Failed to parse one or more of params in conditional: "
               << condition_proto.status() << " "
               << positive_branch_proto.status() << " "
               << negative_branch_proto.status();
    return nullptr;
  }

  absl::StatusOr<v0::Value> conditional =
      CreateConditional(condition_proto.value(), positive_branch_proto.value(),
                        negative_branch_proto.value());
  if (!conditional.ok()) {
    LOG(ERROR) << "Failed to create conditional computation proto: "
               << conditional.status();
    return nullptr;
  }
  const v0::Value computation_proto = conditional.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_custom_function(JNIEnv* env, jstring fn_uri) {
  std::string fn_uri_str = GetString(env, fn_uri);
  absl::string_view fn_uri_view(fn_uri_str);
  absl::StatusOr<v0::Value> custom_function = CreateCustomFunction(fn_uri_view);
  if (!custom_function.ok()) {
    LOG(ERROR) << "Failed to create custom function: "
               << custom_function.status();
    return nullptr;
  }
  const v0::Value computation_proto = custom_function.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_inja_template(JNIEnv* env, jstring template_str) {
  std::string template_string = GetString(env, template_str);
  absl::string_view template_str_view(template_string);
  absl::StatusOr<v0::Value> inja_template =
      CreateInjaTemplate(template_str_view);
  if (!inja_template.ok()) {
    LOG(ERROR) << "Failed to create inja template: " << inja_template.status();
    return nullptr;
  }
  const v0::Value computation_proto = inja_template.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_parallel_map(JNIEnv* env, jbyteArray map) {
  absl::StatusOr<v0::Value> map_proto =
      ParseProtoFromByteArray<v0::Value>(env, map);
  if (!map_proto.ok()) {
    LOG(ERROR) << "Failed to parse map parameter: " << map_proto.status();
    return nullptr;
  }
  absl::StatusOr<v0::Value> parallel_map = CreateParallelMap(map_proto.value());
  if (!parallel_map.ok()) {
    LOG(ERROR) << "Failed to create parallel map: " << parallel_map.status();
    return nullptr;
  }
  const v0::Value computation_proto = parallel_map.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_serial_chain(JNIEnv* env, jobject list) {
  std::vector<v0::Value> value_list = GetValueVectorFromJavaList(env, list);
  if (value_list.empty()) {
    return nullptr;
  }
  absl::StatusOr<v0::Value> serial_chain = CreateSerialChain(value_list);
  if (!serial_chain.ok()) {
    LOG(ERROR) << "Failed to create serial chain computation proto: "
               << serial_chain.status();
    return nullptr;
  }
  const v0::Value computation_proto = serial_chain.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_struct(JNIEnv* env, jobject list) {
  std::vector<v0::Value> value_list = GetValueVectorFromJavaList(env, list);
  if (value_list.empty()) {
    return nullptr;
  }
  absl::StatusOr<v0::Value> struct_proto = CreateStruct(value_list);
  if (!struct_proto.ok()) {
    LOG(ERROR) << "Failed to create struct computation proto: "
               << struct_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = struct_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_call(JNIEnv* env, jbyteArray fn, jbyteArray arg) {
  absl::StatusOr<v0::Value> fn_proto =
      ParseProtoFromByteArray<v0::Value>(env, fn);
  absl::StatusOr<v0::Value> arg_proto =
      ParseProtoFromByteArray<v0::Value>(env, arg);
  if (!fn_proto.ok() || !arg_proto.ok()) {
    LOG(ERROR) << "Failed to parse one or more of params in create_call: "
               << fn_proto.status() << " " << arg_proto.status();
    return nullptr;
  }

  absl::StatusOr<v0::Value> call_proto =
      CreateCall(fn_proto.value(), arg_proto.value());
  if (!call_proto.ok()) {
    LOG(ERROR) << "Failed to create call computation proto: "
               << call_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = call_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_wolfram_alpha(JNIEnv* env, jstring app_id) {
  std::string app_id_str = GetString(env, app_id);
  absl::string_view app_id_view(app_id_str);
  absl::StatusOr<v0::Value> wolfram_alpha = CreateWolframAlpha(app_id_view);
  if (!wolfram_alpha.ok()) {
    LOG(ERROR) << "Failed to create wolfram alpha: " << wolfram_alpha.status();
    return nullptr;
  }
  const v0::Value computation_proto = wolfram_alpha.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_repeat(JNIEnv* env, jint num_steps, jbyteArray body_fn) {
  int c_num_steps = (int)num_steps;
  absl::StatusOr<v0::Value> body_fn_proto =
      ParseProtoFromByteArray<v0::Value>(env, body_fn);
  if (!body_fn_proto.ok()) {
    LOG(ERROR) << "Failed to parse body_fn param in create_repeat: "
               << body_fn_proto.status();
    return nullptr;
  }
  absl::StatusOr<v0::Value> repeat_proto =
      CreateRepeat(c_num_steps, body_fn_proto.value());

  if (!repeat_proto.ok()) {
    LOG(ERROR) << "Failed to create repeat: " << repeat_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = repeat_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_lambda(JNIEnv* env, jstring arg_name, jbyteArray body) {
  std::string arg_name_str = GetString(env, arg_name);
  absl::string_view arg_name_view(arg_name_str);
  absl::StatusOr<v0::Value> body_proto =
      ParseProtoFromByteArray<v0::Value>(env, body);
  if (!body_proto.ok()) {
    LOG(ERROR) << "Failed to parse body param in create_lambda:  "
               << body_proto.status();
    return nullptr;
  }
  absl::StatusOr<v0::Value> lambda_proto =
      CreateLambda(arg_name_view, body_proto.value());

  if (!lambda_proto.ok()) {
    LOG(ERROR) << "Failed to create lambda: " << lambda_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = lambda_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_reference(JNIEnv* env, jstring arg_name) {
  std::string arg_name_str = GetString(env, arg_name);
  absl::string_view arg_name_view(arg_name_str);

  absl::StatusOr<v0::Value> reference_proto = CreateReference(arg_name_view);

  if (!reference_proto.ok()) {
    LOG(ERROR) << "Failed to create reference: " << reference_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = reference_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_regex_partial_match(JNIEnv* env, jstring pattern_str) {
  std::string c_pattern_str = GetString(env, pattern_str);
  absl::string_view pattern_str_view(c_pattern_str);

  absl::StatusOr<v0::Value> regex_partial_match_proto =
      CreateRegexPartialMatch(pattern_str_view);

  if (!regex_partial_match_proto.ok()) {
    LOG(ERROR) << "Failed to create regex_partial_match: "
               << regex_partial_match_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = regex_partial_match_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_logical_not(JNIEnv* env) {
  absl::StatusOr<v0::Value> logical_not_proto = CreateLogicalNot();

  if (!logical_not_proto.ok()) {
    LOG(ERROR) << "Failed to create logical_not: "
               << logical_not_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = logical_not_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_repeated_conditional_chain(JNIEnv* env, jint num_steps,
                                                jobject list) {
  int c_num_steps = (int)num_steps;
  std::vector<v0::Value> value_list = GetValueVectorFromJavaList(env, list);
  if (value_list.empty()) {
    return nullptr;
  }
  absl::StatusOr<v0::Value> repeated_conditional_chain_proto =
      CreateRepeatedConditionalChain(c_num_steps, value_list);
  if (!repeated_conditional_chain_proto.ok()) {
    LOG(ERROR) << "Failed to create repeated_conditional_chain proto: "
               << repeated_conditional_chain_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = repeated_conditional_chain_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_breakable_chain(JNIEnv* env, jobject list) {
  std::vector<v0::Value> value_list = GetValueVectorFromJavaList(env, list);
  if (value_list.empty()) {
    return nullptr;
  }
  absl::StatusOr<v0::Value> breakable_chain_proto =
      CreateBreakableChain(value_list);
  if (!breakable_chain_proto.ok()) {
    LOG(ERROR) << "Failed to create breakable_chain proto: "
               << breakable_chain_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = breakable_chain_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_logger(JNIEnv* env) {
  absl::StatusOr<v0::Value> logger_proto = CreateLogger();

  if (!logger_proto.ok()) {
    LOG(ERROR) << "Failed to create logger: " << logger_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = logger_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_selection(JNIEnv* env, jbyteArray source, jint index) {
  absl::StatusOr<v0::Value> source_proto =
      ParseProtoFromByteArray<v0::Value>(env, source);
  if (!source_proto.ok()) {
    LOG(ERROR) << "Failed to parse source param in create_selection: "
               << source_proto.status();
    return nullptr;
  }
  int c_index = (int)index;

  absl::StatusOr<v0::Value> selection_proto =
      CreateSelection(source_proto.value(), c_index);

  if (!selection_proto.ok()) {
    LOG(ERROR) << "Failed to create selection: " << selection_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = selection_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_lambda_for_conditional(JNIEnv* env, jbyteArray condition,
                                            jbyteArray positive_branch,
                                            jbyteArray negative_branch) {
  absl::StatusOr<v0::Value> condition_proto =
      ParseProtoFromByteArray<v0::Value>(env, condition);
  absl::StatusOr<v0::Value> positive_branch_proto =
      ParseProtoFromByteArray<v0::Value>(env, positive_branch);
  absl::StatusOr<v0::Value> negative_branch_proto =
      ParseProtoFromByteArray<v0::Value>(env, negative_branch);
  if (!condition_proto.ok() || !positive_branch_proto.ok() ||
      !negative_branch_proto.ok()) {
    LOG(ERROR) << "Failed to parse one or more params in "
                  "create_lambda_for_conditional: "
               << condition_proto.status() << " "
               << positive_branch_proto.status() << " "
               << negative_branch_proto.status();
    return nullptr;
  }

  absl::StatusOr<v0::Value> lambda_for_conditional = CreateLambdaForConditional(
      condition_proto.value(), positive_branch_proto.value(),
      negative_branch_proto.value());
  if (!lambda_for_conditional.ok()) {
    LOG(ERROR) << "Failed to create lambda_for_conditional proto: "
               << lambda_for_conditional.status();
    return nullptr;
  }
  const v0::Value computation_proto = lambda_for_conditional.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

jbyteArray GC_create_while(JNIEnv* env, jbyteArray condition_fn,
                           jbyteArray body_fn) {
  absl::StatusOr<v0::Value> condition_fn_proto =
      ParseProtoFromByteArray<v0::Value>(env, condition_fn);
  absl::StatusOr<v0::Value> body_fn_proto =
      ParseProtoFromByteArray<v0::Value>(env, body_fn);
  if (!condition_fn_proto.ok() || !body_fn_proto.ok()) {
    LOG(ERROR) << "Failed to parse one or more params in create_while params: "
               << condition_fn_proto.status() << " " << body_fn_proto.status();
    return nullptr;
  }

  absl::StatusOr<v0::Value> while_proto =
      CreateWhile(condition_fn_proto.value(), body_fn_proto.value());
  if (!while_proto.ok()) {
    LOG(ERROR) << "Failed to create while proto: " << while_proto.status();
    return nullptr;
  }
  const v0::Value computation_proto = while_proto.value();
  absl::StatusOr<jbyteArray> computation_bytes =
      GetJbyteArrayFromProto(env, computation_proto);
  return GetValueOrLogError(computation_bytes,
                            static_cast<jbyteArray>(nullptr));
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateModelInference(  // NOLINT
    JNIEnv* env, jobject obj, jstring model_uri) {
  return GC_create_model_inference(env, model_uri);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateModelInferenceWithConfig(  // NOLINT
    JNIEnv* env, jobject obj, jstring model_uri, jbyteArray config) {
  return GC_create_model_inference_with_config(env, model_uri, config);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreatePromptTemplate(  // NOLINT
    JNIEnv* env, jobject obj, jstring template_str) {
  return GC_create_prompt_template(env, template_str);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateFallback(  // NOLINT
    JNIEnv* env, jobject obj, jobject list) {
  return GC_create_fallback(env, list);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateConditional(  // NOLINT
    JNIEnv* env, jobject obj, jbyteArray condition, jbyteArray positive_branch,
    jbyteArray negative_branch) {
  return GC_create_conditional(env, condition, positive_branch,
                               negative_branch);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateCustomFunction(  // NOLINT
    JNIEnv* env, jobject obj, jstring fn_uri) {
  return GC_create_custom_function(env, fn_uri);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateInjaTemplate(  // NOLINT
    JNIEnv* env, jobject obj, jstring template_str) {
  return GC_create_inja_template(env, template_str);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateParallelMap(  // NOLINT
    JNIEnv* env, jobject obj, jbyteArray map) {
  return GC_create_parallel_map(env, map);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateSerialChain(  // NOLINT
    JNIEnv* env, jobject obj, jobject list) {
  return GC_create_serial_chain(env, list);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateStruct(  // NOLINT
    JNIEnv* env, jobject obj, jobject list) {
  return GC_create_struct(env, list);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateCall(  // NOLINT
    JNIEnv* env, jobject obj, jbyteArray fn, jbyteArray arg) {
  return GC_create_call(env, fn, arg);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateWolframAlpha(  // NOLINT
    JNIEnv* env, jobject obj, jstring app_id) {
  return GC_create_wolfram_alpha(env, app_id);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateRepeat(  // NOLINT
    JNIEnv* env, jobject obj, jint num_steps, jbyteArray body_fn) {
  return GC_create_repeat(env, num_steps, body_fn);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateLambda(  // NOLINT
    JNIEnv* env, jobject obj, jstring arg_name, jbyteArray body) {
  return GC_create_lambda(env, arg_name, body);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateReference(  // NOLINT
    JNIEnv* env, jobject obj, jstring arg_name) {
  return GC_create_reference(env, arg_name);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateRegexPartialMatch(  // NOLINT
    JNIEnv* env, jobject obj, jstring pattern_str) {
  return GC_create_regex_partial_match(env, pattern_str);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateLogicalNot(  // NOLINT
    JNIEnv* env, jobject obj) {
  return GC_create_logical_not(env);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateRepeatedConditionalChain(  // NOLINT
    JNIEnv* env, jobject obj, jint num_steps, jobject list) {
  return GC_create_repeated_conditional_chain(env, num_steps, list);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateBreakableChain(  // NOLINT
    JNIEnv* env, jobject obj, jobject list) {
  return GC_create_breakable_chain(env, list);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateLogger(  // NOLINT
    JNIEnv* env, jobject obj) {
  return GC_create_logger(env);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateSelection(  // NOLINT
    JNIEnv* env, jobject obj, jbyteArray source, jint index) {
  return GC_create_selection(env, source, index);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateLambdaForConditional(  // NOLINT
    JNIEnv* env, jobject obj, jbyteArray condition, jbyteArray positive_branch,
    jbyteArray negative_branch) {
  return GC_create_lambda_for_conditional(env, condition, positive_branch,
                                          negative_branch);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_org_genc_authoring_Constructor_nativeCreateWhile(  // NOLINT
    JNIEnv* env, jobject obj, jbyteArray condition_fn, jbyteArray body_fn) {
  return GC_create_while(env, condition_fn, body_fn);
}

}  // namespace genc
