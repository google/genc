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

#ifndef GENC_CC_INTEROP_OAK_ATTESTATION_PROVIDER_H_
#define GENC_CC_INTEROP_OAK_ATTESTATION_PROVIDER_H_

#include <string>

#include "absl/status/statusor.h"
#include "proto/session/messages.pb.h"

namespace genc {
namespace interop {
namespace oak {

class AttestationProvider {
 public:
  virtual absl::StatusOr<::oak::session::v1::EndorsedEvidence>
      GetEndorsedEvidence(const std::string& serialized_public_key) = 0;

  virtual ~AttestationProvider() {}
};

}  // namespace oak
}  // namespace interop
}  // namespace genc

#endif  // GENC_CC_INTEROP_OAK_ATTESTATION_PROVIDER_H_
