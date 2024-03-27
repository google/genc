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

#ifndef GENERATIVE_COMPUTING_CC_INTRINSICS_DELEGATE_H_
#define GENERATIVE_COMPUTING_CC_INTRINSICS_DELEGATE_H_

#include <functional>
#include <optional>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/intrinsics/intrinsic_uris.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

class Delegate : public ControlFlowIntrinsicHandlerBase {
 public:
  // Handler function that takes the computation to invoke followed by an
  // optional dynamic argument, and a pointer to the result to fill in.
  typedef std::function<absl::Status(
      v0::Value, std::optional<v0::Value>, v0::Value*)> RunnerFn;

  // A set of runners for various runtime environments.
  typedef absl::flat_hash_map<std::string, RunnerFn> RunnerMap;

  Delegate(const RunnerMap& runner_map)
      : ControlFlowIntrinsicHandlerBase(kDelegate),
        runner_map_(runner_map) {}

  virtual ~Delegate() {}

  absl::Status CheckWellFormed(const v0::Intrinsic& intrinsic_pb) const final;

  absl::StatusOr<ValueRef> ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                                       std::optional<ValueRef> arg,
                                       Context* context) const final;

 private:
  const RunnerMap runner_map_;
};

}  // namespace intrinsics
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_INTRINSICS_DELEGATE_H_
