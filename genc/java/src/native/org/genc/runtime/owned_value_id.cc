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

#include "genc/c/runtime/c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL
Java_org_genc_runtime_OwnedValueId_ref(
    JNIEnv* env, jclass clazz, jlong value_handle) {
  GC_OwnedValueId* owned_value_id =
      reinterpret_cast<GC_OwnedValueId*>(value_handle);
  return GC_GetValue(owned_value_id);
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
