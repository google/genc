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

#include <cstddef>
#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include "genc/cc/interop/confidential_computing/attestation.h"
#include "proto/session/messages.pb.h"

namespace genc {
namespace interop {
namespace confidential_computing {
namespace {

constexpr char kLocalhostTokenUrl[] = "http://localhost/v1/token";
constexpr char kLauncherSocketPath[] = "/run/container_launcher/teeserver.sock";

size_t WriteCallback(
    void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t totalSize = size * nmemb;
  output->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}

class AttestationProviderImpl : public AttestationProvider {
 public:
  absl::StatusOr<::oak::session::v1::EndorsedEvidence>
      GetEndorsedEvidence() override {
    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
      return absl::InternalError(
          "Attestation provider unable to init CURL.");
    }
    curl_easy_setopt(curl, CURLOPT_URL, kLocalhostTokenUrl);
    curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, kLauncherSocketPath);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    std::string response_json;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_json);
    CURLcode curl_code = curl_easy_perform(curl);
    if (curl_code != CURLE_OK) {
      return absl::InternalError(absl::StrCat(
          "Attestation provider's GetEndorsedEvidence implementation has "
          "received an error from the local launcher call: \"",
          curl_easy_strerror(curl_code),
          "\"."));
    }
    curl_easy_cleanup(curl);

    // TODO(b/333410413): Implement the rest of this, as per the below, and
    // package it in Oak's attestation data structure.
    // https://cloud.google.com/confidential-computing/confidential-space/docs/connect-external-resources#unencrypted

    return absl::UnimplementedError(absl::StrCat(
        "GetEndorsedEvidence has not been fully implemented yet, but here's "
        "the JSON returned by the attestation service: ", response_json));
  }

  virtual ~AttestationProviderImpl() {};
};

}  // namespace

absl::StatusOr<std::shared_ptr<AttestationProvider>>
CreateAttestationProvider() {
  return std::make_shared<AttestationProviderImpl>();
}

}  // namespace confidential_computing
}  // namespace interop
}  // namespace genc
