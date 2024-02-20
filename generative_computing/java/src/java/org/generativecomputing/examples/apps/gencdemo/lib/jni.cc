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

#include <jni.h>

#include "absl/log/log.h"
#include "absl/status/statusor.h"
#include "generative_computing/c/runtime/c_api.h"
#include "generative_computing/c/runtime/c_api_internal.h"  // IWYU pragma: keep
#include "generative_computing/cc/runtime/android/android_executor_stacks.h"

namespace generative_computing {

namespace {
static jobject open_ai_client_global_ref = nullptr;
static jobject google_ai_client_global_ref = nullptr;
static jobject llm_inference_client_global_ref = nullptr;

}  // namespace

GC_Executor* GC_create_android_executor(JavaVM* jvm, JNIEnv* env,
                                        jobject open_ai_client,
                                        jobject google_ai_client,
                                        jobject llm_inference_client) {
  // Create global references to refer from native threads.
  open_ai_client_global_ref = env->NewGlobalRef(open_ai_client);
  if (open_ai_client_global_ref == nullptr) {
    LOG(ERROR) << "Couldn't create global reference for OpenAI client";
    return nullptr;
  }
  google_ai_client_global_ref = env->NewGlobalRef(google_ai_client);
  if (google_ai_client_global_ref == nullptr) {
    LOG(ERROR) << "Couldn't create global reference for GoogleAI client";
    return nullptr;
  }

  if (llm_inference_client != nullptr) {
    llm_inference_client_global_ref = env->NewGlobalRef(llm_inference_client);
    if (llm_inference_client_global_ref == nullptr) {
      LOG(ERROR) << "Couldn't create global reference for Mediapipe "
                    "LlmInference client";
      return nullptr;
    }
  }
  auto lamda_executor = generative_computing::CreateAndroidExecutor(
      jvm, open_ai_client_global_ref, google_ai_client_global_ref,
      llm_inference_client_global_ref);
  GC_Executor* e = new GC_Executor(lamda_executor.value());
  return e;
}

extern "C" jlong
Java_org_generativecomputing_examples_apps_gencdemo_DefaultAndroidExecutor_createAndroidExecutor(  // NOLINT
    JNIEnv* env, jobject obj, jobject open_ai_client, jobject google_ai_client,
    jobject llm_inference_client) {
  JavaVM* jvm;
  env->GetJavaVM(&jvm);
  return reinterpret_cast<jlong>(GC_create_android_executor(
      jvm, env, open_ai_client, google_ai_client, llm_inference_client));
}

extern "C" void
Java_org_generativecomputing_examples_apps_gencdemo_DefaultAndroidExecutor_cleanupAndroidExecutorState(  // NOLINT
    JNIEnv* env, jobject obj) {
  if (open_ai_client_global_ref != nullptr) {
    env->DeleteGlobalRef(open_ai_client_global_ref);
  }
  if (llm_inference_client_global_ref != nullptr) {
    env->DeleteGlobalRef(llm_inference_client_global_ref);
  }
}

}  // namespace generative_computing
