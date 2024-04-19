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

#ifndef GENC_CC_INTEROP_OAK_SERVER_H_
#define GENC_CC_INTEROP_OAK_SERVER_H_

#include <memory>

#include "absl/status/statusor.h"
#include "genc/cc/interop/oak/attestation_provider.h"
#include "genc/proto/v0/executor.grpc.pb.h"
#include "proto/session/service_unary.grpc.pb.h"

namespace genc {
namespace interop {
namespace oak {

// Creates an Oak transport-compatible  service endpoint.
absl::StatusOr<std::unique_ptr<::oak::session::v1::UnarySession::Service>>
CreateService(
    std::shared_ptr<v0::Executor::Service> executor_service,
    std::shared_ptr<AttestationProvider> attestation_provider);

}  // namespace oak
}  // namespace interop
}  // namespace genc

#endif  // GENC_CC_INTEROP_OAK_SERVER_H_
