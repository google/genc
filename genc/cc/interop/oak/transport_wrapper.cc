/* Copyright 2024, The GenC Authors.

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

#include <memory>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "genc/cc/interop/oak/transport_wrapper.h"
#include "cc/transport/transport.h"

namespace genc {
namespace {

class TransportWrapperImpl : public oak::transport::TransportWrapper {
 public:
  TransportWrapperImpl() {}

  ~TransportWrapperImpl() override {}

  absl::StatusOr<oak::session::v1::EndorsedEvidence>
  GetEndorsedEvidence() override {
    // TODO(b/333410413): Do something.
    return absl::UnimplementedError("This functionality is not implemented.");
  }

  absl::StatusOr<oak::crypto::v1::EncryptedResponse> Invoke(
      const oak::crypto::v1::EncryptedRequest& encrypted_request) override {
    // TODO(b/333410413): Do something.
    return absl::UnimplementedError("This functionality is not implemented.");
  }

 private:
};

}  // namespace

absl::StatusOr<std::unique_ptr<oak::transport::TransportWrapper>>
CreateOakTransportWrapper() {
  return std::make_unique<genc::TransportWrapperImpl>();
}

}  // namespace genc
