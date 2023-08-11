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

#ifndef GENERATIVE_COMPUTING_CC_RUNTIME_MODEL_CALL_EXECUTOR_H_
#define GENERATIVE_COMPUTING_CC_RUNTIME_MODEL_CALL_EXECUTOR_H_

namespace generative_computing {

// Executor that specializes in handling model calls (i.e., that only handles
// the `ModelCall` computations as defined in the `computation.proto`, and
// routes to the appropriate model backends depending on the model declared).
class ModelCallExecutor {
 public:
  // TODO(b/295041601): Inherit the core interfaces here, once defined.

  // TODO(b/295260921): Based on a prefix of the URI embedded in the modelcall,
  // route calls to an appropriate child executor or child component that
  // specializes in handling model calls for the corresponding class of models.
  // The set of supported models and child executors is something that should
  // come in as a parameter (since we want it to be a point of flexibility we
  // offer to whoever is handling runtime deployment and configuration).
  // In particular, we shoul be able to effectively route the calls to either
  // an on-device or a Cloud executor (and in general, there could be multiple
  // of each suported here within the same runtime stack).

  virtual ~ModelCallExecutor() {}
};

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_MODEL_CALL_EXECUTOR_H_
