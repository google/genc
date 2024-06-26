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

#include "genc/java/src/jni/jni_utils.h"

#include <jni.h>

#include <string>

#include "absl/log/log.h"

namespace genc {

void ThrowException(const std::string& exception_class,
                    const std::string& message, JNIEnv* env) {
  jclass jexception_class = env->FindClass(exception_class.c_str());
  env->ThrowNew(jexception_class, message.c_str());
}

void ThrowRuntimeException(const std::string& message, JNIEnv* env) {
  ThrowException("java/lang/RuntimeException", message, env);
}

std::string GetString(JNIEnv* env, jstring java_string) {
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

JNIEnv* GetThreadLocalJniEnv(JavaVM* jvm) {
  if (jvm == nullptr) {
    return nullptr;  // No JavaVM provided to get JNIEnv from.
  }
  JNIEnv* env = nullptr;
  jint error = jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
  if (error != JNI_OK) {
    return nullptr;
  }
  return env;
}

JNIEnv* GetJniEnv(JavaVM* jvm) {
  JNIEnv* env = GetThreadLocalJniEnv(jvm);
  if (env == nullptr) {
    // No JNI environment on native thread, attach the native thread to JVM
    int status = jvm->AttachCurrentThread((void**)&env, nullptr);
    if (status != JNI_OK) {
      LOG(ERROR) << "Current thread attachment to JVM failed.";
      return nullptr;
    }
  }
  return env;
}

}  // namespace genc
