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

#ifndef GENC_CC_INTEROP_CONFIDENTIAL_COMPUTING_ATTESTATION_H_
#define GENC_CC_INTEROP_CONFIDENTIAL_COMPUTING_ATTESTATION_H_

#include <memory>

#include "absl/status/statusor.h"
#include "genc/cc/interop/oak/attestation_provider.h"

namespace genc {
namespace interop {
namespace confidential_computing {

class AttestationProvider : public oak::AttestationProvider {};

absl::StatusOr<std::shared_ptr<AttestationProvider>>
    CreateAttestationProvider();

}  // namespace confidential_computing
}  // namespace interop
}  // namespace genc

#endif  // GENC_CC_INTEROP_CONFIDENTIAL_COMPUTING_ATTESTATION_H_
