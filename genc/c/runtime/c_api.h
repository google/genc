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

#ifndef GENC_C_RUNTIME_C_API_H_
#define GENC_C_RUNTIME_C_API_H_

#include <stddef.h>
#include <stdint.h>

#include "genc/c/runtime/c_api_macros.h"
#include "genc/c/runtime/gc_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GC_Executor GC_Executor;
typedef struct GC_OwnedValueId GC_OwnedValueId;

GC_CAPI_EXPORT extern uint64_t GC_GetValue(GC_OwnedValueId* owned_value_id);

GC_CAPI_EXPORT extern GC_Executor* GC_create();

GC_CAPI_EXPORT extern GC_OwnedValueId* GC_create_value(GC_Executor* executor,
                                                       GC_Buffer* value);

GC_CAPI_EXPORT extern GC_OwnedValueId* GC_create_call(GC_Executor* executor,
                                                      uint64_t function,
                                                      uint64_t arg);

GC_CAPI_EXPORT extern void GC_materialize(GC_Executor* executor, uint64_t value,
                                          GC_Buffer* out);

GC_CAPI_EXPORT extern void GC_dispose(GC_Executor* executor, uint64_t value);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif  // GENC_C_RUNTIME_C_API_H_
