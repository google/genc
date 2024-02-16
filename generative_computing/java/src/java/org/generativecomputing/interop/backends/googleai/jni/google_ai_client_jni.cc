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
#include "generative_computing/java/src/java/org/generativecomputing/interop/backends/googleai/jni/google_ai_client_jni.h"

#include <jni.h>

#include <string>

#include "absl/log/check.h"
#include "absl/log/log.h"

namespace generative_computing {

namespace {
// TODO(pagarwl): Factor out helper Jni functions into utils and share across
// other backend interops.
void ThrowException(const std::string& exception_class,
                    const std::string& message, JNIEnv* env) {
  jclass jexception_class = env->FindClass(exception_class.c_str());
  env->ThrowNew(jexception_class, message.c_str());
}

void ThrowRuntimeException(const std::string& message, JNIEnv* env) {
  ThrowException("java/lang/RuntimeException", message, env);
}

static std::string GetString(JNIEnv* env, jstring java_string) {
  jboolean is_copy;
  const char* name_pointer =
      java_string ? env->GetStringUTFChars(java_string, &is_copy) : nullptr;
  if (name_pointer == nullptr) {
    return "";
  }
  std::string result(name_pointer);
  env->ReleaseStringUTFChars(java_string, name_pointer);
  return result;
}

jbyteArray GetJbyteArrayFromString(JNIEnv* env, const std::string& string) {
  jsize length = static_cast<jsize>(string.length());
  jbyteArray javaByteArray = env->NewByteArray(length);

  // Set the elements of the Java byte array with the C++ string data
  env->SetByteArrayRegion(javaByteArray, 0, length,
                          reinterpret_cast<const jbyte*>(string.c_str()));
  return javaByteArray;
}

JNIEnv* GetThreadLocalJniEnv(JavaVM* vm) {
  if (vm == nullptr) {
    return nullptr;  // No JavaVM provided to get JNIEnv from.
  }
  JNIEnv* env = nullptr;
  jint error = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
  if (error != JNI_OK) {
    LOG(INFO) << "Error trying to get JNIEnv* on local thread. Error code: "
              << error;
    return nullptr;
  }
  return env;
}

}  // namespace

std::string CallGoogleAiClient(JavaVM* jvm, jobject google_ai_client,
                               std::string model_config, std::string request) {
  CHECK(jvm != nullptr) << "JVM is null";
  JNIEnv* env = GetThreadLocalJniEnv(jvm);
  if (env == nullptr) {
    LOG(INFO) << "No JNIEnv on native thread, need to attach it to JVM";
#ifdef __ANDROID__
    int status = jvm->AttachCurrentThread(&env, nullptr);
#else
    int status = jvm->AttachCurrentThread((void**)&env, nullptr);
#endif
    if (status != JNI_OK) {
      LOG(ERROR) << "Current thread attachment to JVM failed";
      return "";
    }
  }

  if (env == nullptr) {
    LOG(ERROR) << "Couldn't get JNI env";
    return "";
  }

  jclass googleAiClientClass = env->GetObjectClass(google_ai_client);
  if (googleAiClientClass == nullptr) {
    ThrowRuntimeException("Couldn't retrieve Google AI client class from JNI",
                          env);
  }
  jmethodID callMethodId =
      env->GetMethodID(googleAiClientClass, "call", "([B[B)Ljava/lang/String;");

  if (callMethodId == 0) {
    ThrowRuntimeException("Couldn't retrieve method id from JNI", env);
  }

  jbyteArray jmodel_config = GetJbyteArrayFromString(env, model_config);
  jbyteArray jrequest = GetJbyteArrayFromString(env, request);
  jobject response_string = env->CallObjectMethod(
      google_ai_client, callMethodId, jmodel_config, jrequest);
  std::string response = GetString(env, (jstring)response_string);
  env->DeleteLocalRef(response_string);
  env->DeleteLocalRef(jrequest);
  env->DeleteLocalRef(jmodel_config);
  jvm->DetachCurrentThread();
  return response;
}

}  // namespace generative_computing
