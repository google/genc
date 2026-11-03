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

#include "genc/cc/intrinsics/confidential_computation.h"

#include <memory>
#include <string>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "genc/cc/interop/networking/curl_based_http_client.h"
#include "genc/cc/interop/networking/http_client_interface.h"
#include "genc/cc/intrinsics/intrinsic_uris.h"
#include "genc/cc/runtime/executor.h"
#include "genc/cc/runtime/intrinsic_handler.h"
#include "genc/cc/runtime/remote_executor.h"
#include "genc/cc/runtime/status_macros.h"
#include "genc/proto/v0/computation.pb.h"
#include "include/grpcpp/channel.h"
#include "include/grpcpp/create_channel.h"
#include "include/grpcpp/security/credentials.h"

namespace genc {
namespace intrinsics {
namespace {

constexpr char kServerAddressLabel[] = "server_address";
constexpr char kImageDigestLabel[] = "image_digest";

absl::StatusOr<std::shared_ptr<interop::networking::HttpClientInterface>>
PassOrCreateDefaultHttpClient(
    std::shared_ptr<interop::networking::HttpClientInterface>
    http_client_interface) {
  if (http_client_interface != nullptr) {
    return http_client_interface;
  }
  return interop::networking::CreateCurlBasedHttpClient(false);
}

}  // namespace

ConfidentialComputation::ConfidentialComputation(
    std::shared_ptr<interop::networking::HttpClientInterface>
    http_client_interface)
    : InlineIntrinsicHandlerBase(kConfidentialComputation),
      http_client_interface_(
      PassOrCreateDefaultHttpClient(http_client_interface)) { }

absl::Status ConfidentialComputation::CheckWellFormed(
    const v0::Intrinsic& intrinsic_pb) const {
  if (!intrinsic_pb.static_parameter().has_struct_()) {
    return absl::InvalidArgumentError(
        "Expected a struct as a static parameter.");
  }
  if (intrinsic_pb.static_parameter().struct_().element_size() != 2) {
    return absl::InvalidArgumentError(
        "Expected 2 elements in the static parameter struct.");
  }
  auto config = intrinsic_pb.static_parameter().struct_().element(1);
  if (!config.has_struct_()) {
    return absl::InvalidArgumentError(
        "Expected a struct as the second element in the static parameter.");
  }
  for (const auto& element : config.struct_().element()) {
    if (!element.has_str()) {
      return absl::InvalidArgumentError(
          "Expected a string as an element of the static parameter.");
    }
    if (element.str().empty()) {
      return absl::InvalidArgumentError(
          "Expected a non-empty string as an element of the static parameter.");
    }
    if (element.label().empty()) {
      return absl::InvalidArgumentError(
          "Expected all elements in the static parameter to have labels.");
    }
    if ((element.label() != kServerAddressLabel) &&
        (element.label() != kImageDigestLabel)) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Unrecognized label in the static parameter: \"",
          element.label(), "\"."));
    }
  }
  return absl::OkStatus();
}

absl::Status ConfidentialComputation::ExecuteCall(
    const v0::Intrinsic& intrinsic_pb, const v0::Value& arg, v0::Value* result,
    Context* context) const {
  return absl::UnimplementedError(
      "ConfidentialComputation with Oak UnarySession is deprecated "
      "(b/426366723).");
}

}  // namespace intrinsics
}  // namespace genc
