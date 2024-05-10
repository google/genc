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

#ifndef GENC_CC_AUTHORING_TRACING_CONTEXT_H_
#define GENC_CC_AUTHORING_TRACING_CONTEXT_H_

#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "genc/cc/modules/retrieval/bi_map.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

// An authoring tracing context for invocations of the `Computation` class.`
class TracingContext {
 public:
  TracingContext() = default;
  ~TracingContext() = default;

  // Processed the call to `portable_ir` in this authoring context.
  absl::StatusOr<v0::Value> Call(const v0::Value& portable_ir,
                                 const std::vector<v0::Value>& args);
  // Returns local variables.
  const BiMap<std::string, std::string>& locals() { return name_to_ir_; }

 private:
  // IR stands for intermediate representation.
  BiMap<std::string, std::string> name_to_ir_;
  absl::StatusOr<v0::Value> ReplaceWithReference(const v0::Value& ir);
};

}  // namespace genc

#endif  // GENC_CC_AUTHORING_TRACING_CONTEXT_H_
