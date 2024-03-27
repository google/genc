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

#ifndef GENERATIVE_COMPUTING_C_RUNTIME_C_API_INTERNAL_H_
#define GENERATIVE_COMPUTING_C_RUNTIME_C_API_INTERNAL_H_

#include <memory>
#include <utility>

#include "generative_computing/cc/runtime/executor.h"

struct GC_Executor {
  GC_Executor(std::shared_ptr<generative_computing::Executor> e)
      : executor(e) {}

  std::shared_ptr<generative_computing::Executor> executor;
};

struct GC_OwnedValueId {
  GC_OwnedValueId(generative_computing::OwnedValueId v)
      : owned_value_id(std::move(v)) {}

  generative_computing::OwnedValueId owned_value_id;
};

#endif  // GENERATIVE_COMPUTING_C_RUNTIME_C_API_INTERNAL_H_
