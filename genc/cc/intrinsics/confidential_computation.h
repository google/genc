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

#ifndef GENC_CC_INTRINSICS_CONFIDENTIAL_COMPUTATION_H_
#define GENC_CC_INTRINSICS_CONFIDENTIAL_COMPUTATION_H_

#include <memory>
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "genc/cc/interop/networking/http_client_interface.h"
#include "genc/cc/runtime/intrinsic_handler.h"
#include "genc/proto/v0/computation.pb.h"

namespace genc {
namespace intrinsics {

class ConfidentialComputation : public InlineIntrinsicHandlerBase {
 public:
  explicit ConfidentialComputation(
      std::shared_ptr<interop::networking::HttpClientInterface>
          http_client_interface);

  virtual ~ConfidentialComputation() {}

  absl::Status CheckWellFormed(const v0::Intrinsic& intrinsic_pb) const final;
  absl::Status ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                           const v0::Value& arg, v0::Value* result,
                           Context* context) const final;

 private:
  const absl::StatusOr<
    std::shared_ptr<interop::networking::HttpClientInterface>>
    http_client_interface_;
};

}  // namespace intrinsics
}  // namespace genc

#endif  // GENC_CC_INTRINSICS_CONFIDENTIAL_COMPUTATION_H_
