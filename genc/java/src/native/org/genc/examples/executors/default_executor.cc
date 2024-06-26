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

#include "absl/log/log.h"
#include "absl/status/statusor.h"
#include "genc/c/runtime/c_api.h"
#include "genc/c/runtime/c_api_internal.h"  // IWYU pragma: keep
#include "genc/cc/examples/executors/java_executor_stacks.h"

namespace genc {

namespace {
static jobject open_ai_client_global_ref = nullptr;
static jobject google_ai_client_global_ref = nullptr;
static jobject wolfram_alpha_client_global_ref = nullptr;

}  // namespace

GC_Executor* GC_create_default_executor(JavaVM* jvm, JNIEnv* env,
                                        jobject open_ai_client,
                                        jobject google_ai_client,
                                        jobject wolfram_alpha_client) {
  // Create global references to refer from native threads.
  if (open_ai_client != nullptr) {
    open_ai_client_global_ref = env->NewGlobalRef(open_ai_client);
    if (open_ai_client_global_ref == nullptr) {
      LOG(ERROR) << "Couldn't create global reference for OpenAI client";
      return nullptr;
    }
  }

  if (google_ai_client != nullptr) {
    google_ai_client_global_ref = env->NewGlobalRef(google_ai_client);
    if (google_ai_client_global_ref == nullptr) {
      LOG(ERROR) << "Couldn't create global reference for GoogleAI client";
      return nullptr;
    }
  }

  if (wolfram_alpha_client != nullptr) {
    wolfram_alpha_client_global_ref = env->NewGlobalRef(wolfram_alpha_client);
    if (wolfram_alpha_client_global_ref == nullptr) {
      LOG(ERROR) << "Couldn't create global reference for WolframAlpha client";
      return nullptr;
    }
  }
  auto lamda_executor = genc::CreateDefaultExecutor(
      jvm, open_ai_client_global_ref, google_ai_client_global_ref,
      wolfram_alpha_client_global_ref);
  GC_Executor* e = new GC_Executor(lamda_executor.value());
  return e;
}

extern "C" JNIEXPORT jlong JNICALL
Java_org_genc_examples_executors_DefaultExecutor_createDefaultExecutor(  // NOLINT
    JNIEnv* env, jobject obj, jobject open_ai_client, jobject google_ai_client,
    jobject wolfram_alpha_client) {
  JavaVM* jvm;
  env->GetJavaVM(&jvm);
  return reinterpret_cast<jlong>(GC_create_default_executor(
      jvm, env, open_ai_client, google_ai_client, wolfram_alpha_client));
}

extern "C" JNIEXPORT void JNICALL
Java_org_genc_examples_executors_DefaultExecutor_cleanupExecutorState(  // NOLINT
    JNIEnv* env, jobject obj) {
  if (open_ai_client_global_ref != nullptr) {
    env->DeleteGlobalRef(open_ai_client_global_ref);
  }
  if (google_ai_client_global_ref != nullptr) {
    env->DeleteGlobalRef(google_ai_client_global_ref);
  }
  if (wolfram_alpha_client_global_ref != nullptr) {
    env->DeleteGlobalRef(wolfram_alpha_client_global_ref);
  }
}

}  // namespace genc
