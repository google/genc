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

#ifndef GENC_CC_BASE_COMPUTATION_H_
#define GENC_CC_BASE_COMPUTATION_H_

#include <vector>

#include "absl/status/statusor.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

// Traced computation to provide make complex workflow authoring easier.
class Computation {
 public:
  explicit Computation(const v0::Value& portable_ir)
      : portable_ir_(portable_ir) {}

  // Returns the PortableIR of the computation.
  const v0::Value& portable_ir();

  // Calls the computation with args.
  // Provides a more convenient Python like syntax for authoring.
  absl::StatusOr<Computation> operator()(const std::vector<v0::Value>& args);

 private:
  v0::Value portable_ir_;
};

}  // namespace genc

#endif  // GENC_CC_BASE_COMPUTATION_H_
