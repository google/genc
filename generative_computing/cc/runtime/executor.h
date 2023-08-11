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

#ifndef GENERATIVE_COMPUTING_CC_RUNTIME_EXECUTOR_H_
#define GENERATIVE_COMPUTING_CC_RUNTIME_EXECUTOR_H_

namespace generative_computing {

// The basic abstract interface shared among all runtime components.
// Instances of this interface may specialize in a single atomic function,
// e.g., just model calls, or composite processing (e.g., chains) where
// they might delegate to other executor components, or they may represent
// specializations for a given type of environment (e.g., the entire stack
// of executors designed for on-device deployment).
class Executor {
 public:
  // TODO(b/295041601): Develop the core interfaces.

  virtual ~Executor() {}
};

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_EXECUTOR_H_
