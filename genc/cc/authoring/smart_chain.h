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

#ifndef GENC_CC_AUTHORING_SMART_CHAIN_H_
#define GENC_CC_AUTHORING_SMART_CHAIN_H_

#include <stdbool.h>

#include <vector>

#include "absl/status/statusor.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {

// Used to build chains with loops, breakpoints, chain of chains. Will always
// choose the simpler and more efficient chain type based on chained
// computations.

// Chaining Behavior:
// 1. Within a single iteration chain execution, output of previous op is inputs
// for next op.
// 2. If break conditions are set and met, execution will stop and return last
// state before the break.

// Control Flow Behavior:
// 3. If number of iteration is set, the entire chain executed N times. Between
// iterations, output of previous iteration is input to next iteration.
// 4. If is_parallel is set, the entire chain (loop included) will be used
// as a parallel chain. If the constructed chain is denoted as function f(),
// input is// struct(a,b,c), f(a), f(b), f(c) will be executed in parallel, and
// output will output will be struct(f(a), f(b), f(c)).
class SmartChain {
 public:
  explicit SmartChain() {}

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
  void SetIsParallel(bool is_parallel) { is_parallel_ = is_parallel; }

 private:
  bool is_parallel_ = false;
  int num_iteration_ = 0;
  std::vector<v0::Value> chained_ops_;
};
}  // namespace genc

#endif  // GENC_CC_AUTHORING_SMART_CHAIN_H_
