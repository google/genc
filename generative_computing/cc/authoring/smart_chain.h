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

#ifndef GENERATIVE_COMPUTING_CC_AUTHORING_SMART_CHAIN_H_
#define GENERATIVE_COMPUTING_CC_AUTHORING_SMART_CHAIN_H_

#include <vector>

#include "absl/status/statusor.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

// Used to build chains with loops, breakpoints, chain of chains. Will always
// choose the simpler and more efficient chain type based on chained
// computations.

// Behavior:
// 1. Within a single iteration chain execution, output of previous op is inputs
// for next op.
// 2. If number of iteration is set, the entire chain executed N times. Between
// iterations, output of previous iteration is input to next iteration.
// 3. If break conditions are set and met, execution will stop and return last
// state before the break.
class SmartChain {
 public:
  explicit SmartChain(int num_iteration = 0) : num_iteration_(num_iteration) {}

  // Enable Pipe operator op1 | op2 | op3 ...
  SmartChain& operator|(const v0::Value& op) {
    chained_ops_.push_back(op);
    return *this;
  }

  SmartChain& operator|(const absl::StatusOr<v0::Value>& op) {
    chained_ops_.push_back(op.value());
    return *this;
  }

  SmartChain& operator|(SmartChain& other_chain) {
    chained_ops_.push_back(other_chain.Build().value());
    return *this;
  }

  // Sets number of iterations
  SmartChain& operator|(int num_iteration) {
    SetNumIteration(num_iteration);
    return *this;
  }

  absl::StatusOr<v0::Value> Build();

  void SetNumIteration(int i) { num_iteration_ = i; }

 private:
  int num_iteration_;
  std::vector<v0::Value> chained_ops_;
};
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_AUTHORING_SMART_CHAIN_H_
