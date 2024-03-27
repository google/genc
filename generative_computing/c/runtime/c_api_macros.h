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

#ifndef GENERATIVE_COMPUTING_C_RUNTIME_C_API_MACROS_H_
#define GENERATIVE_COMPUTING_C_RUNTIME_C_API_MACROS_H_

#ifdef SWIG
#define GC_CAPI_EXPORT
#else
#if defined(_WIN32)
#ifdef GC_COMPILE_LIBRARY
#define GC_CAPI_EXPORT __declspec(dllexport)
#else
#define GC_CAPI_EXPORT __declspec(dllimport)
#endif  // GC_COMPILE_LIBRARY
#else
#ifdef GC_CAPI_WEAK
#define GC_CAPI_EXPORT \
  __attribute__((visibility("default"))) __attribute((weak))
#else
#define GC_CAPI_EXPORT __attribute__((visibility("default")))
#endif  // GC_CAPI_WEAK
#endif  // _WIN32
#endif  // SWIG

#endif  // GENERATIVE_COMPUTING_C_RUNTIME_C_API_MACROS_H_
