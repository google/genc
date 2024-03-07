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

#ifndef GENERATIVE_COMPUTING_JAVA_SRC_JAVA_ORG_GENERATIVECOMPUTING_INTEROP_JNI_JNI_UTILS_H_
#define GENERATIVE_COMPUTING_JAVA_SRC_JAVA_ORG_GENERATIVECOMPUTING_INTEROP_JNI_JNI_UTILS_H_

#include <jni.h>

#include <string>

namespace generative_computing {

// Throws an exception in the provided JNI environment with the exception class
// and message.
void ThrowException(const std::string& exception_class,
                    const std::string& message, JNIEnv* env);

// Throws a RuntimeException.
void ThrowRuntimeException(const std::string& message, JNIEnv* env);

// Converts a Java string to C++ string.
std::string GetString(JNIEnv* env, jstring java_string);

// Converts a C++ string to Java byte array.
jbyteArray GetJbyteArrayFromString(JNIEnv* env, const std::string& string);

// Returns the thread local JNI environment.
JNIEnv* GetThreadLocalJniEnv(JavaVM* vm);

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_JAVA_SRC_JAVA_ORG_GENERATIVECOMPUTING_INTEROP_JNI_JNI_UTILS_H_
