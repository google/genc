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

#include "absl/status/statusor.h"
#include "genc/cc/examples/executors/executor_stacks.h"
#include "genc/c/runtime/c_api.h"
#include "genc/c/runtime/c_api_internal.h"  // IWYU pragma: keep

namespace genc {

GC_Executor* GC_create_default_executor(JavaVM* jvm, JNIEnv* env) {
  auto executor = genc::CreateDefaultExecutor();
  GC_Executor* const e = new GC_Executor(executor.value());
  return e;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_org_genc_examples_executors_DefaultExecutor_createDefaultExecutor(  // NOLINT
    JNIEnv* env, jobject obj) {
  JavaVM* jvm;
  env->GetJavaVM(&jvm);
  return reinterpret_cast<jlong>(GC_create_default_executor(jvm, env));
}

}  // namespace genc
