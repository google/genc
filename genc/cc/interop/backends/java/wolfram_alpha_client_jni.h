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

#ifndef GENC_CC_INTEROP_BACKENDS_JAVA_WOLFRAM_ALPHA_CLIENT_JNI_H_
#define GENC_CC_INTEROP_BACKENDS_JAVA_WOLFRAM_ALPHA_CLIENT_JNI_H_

#include <jni.h>

#include <string>

namespace genc {

std::string CallWolframAlphaClient(JavaVM *jvm, jobject wolfram_alpha_client,
                                   std::string model_config,
                                   std::string request);

}

#endif  // GENC_CC_INTEROP_BACKENDS_JAVA_WOLFRAM_ALPHA_CLIENT_JNI_H_
