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
#include "genc/cc/interop/backends/java/wolfram_alpha_client_jni.h"


#include <jni.h>

#include <string>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "genc/java/src/jni/jni_utils.h"

namespace genc {

std::string CallWolframAlphaClient(JavaVM* jvm, jobject wolfram_alpha_client,
                                   std::string config, std::string request) {
  CHECK(jvm != nullptr) << "JVM is null";
  JNIEnv* env = GetJniEnv(jvm);
  if (env == nullptr) {
    LOG(ERROR) << "Couldn't get JNI env";
    return "";
  }

  jclass wolframAlphaClientClass = env->GetObjectClass(wolfram_alpha_client);
  if (wolframAlphaClientClass == nullptr) {
    ThrowRuntimeException(
        "Couldn't retrieve Wolfram Alpha client class from JNI", env);
  }
  jmethodID callMethodId = env->GetMethodID(wolframAlphaClientClass, "call",
                                            "([B[B)Ljava/lang/String;");

  if (callMethodId == 0) {
    ThrowRuntimeException("Couldn't retrieve method id from JNI", env);
  }

  jbyteArray jconfig = GetJbyteArrayFromString(env, config);
  jbyteArray jrequest = GetJbyteArrayFromString(env, request);
  jobject response_string = env->CallObjectMethod(
      wolfram_alpha_client, callMethodId, jconfig, jrequest);
  std::string response = GetString(env, (jstring)response_string);
  env->DeleteLocalRef(response_string);
  env->DeleteLocalRef(jrequest);
  env->DeleteLocalRef(jconfig);
  jvm->DetachCurrentThread();
  return response;
}

}  // namespace genc
