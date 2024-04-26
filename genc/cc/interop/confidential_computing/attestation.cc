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

#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include "genc/cc/interop/confidential_computing/attestation.h"
#include "genc/cc/runtime/status_macros.h"
#include <nlohmann/json.hpp>
#include "proto/session/messages.pb.h"
#include "tink/config/global_registry.h"
#include "tink/jwt/jwk_set_converter.h"
#include "tink/jwt/jwt_public_key_verify.h"
#include "tink/jwt/jwt_signature_config.h"
#include "tink/jwt/jwt_validator.h"
#include "tink/jwt/verified_jwt.h"
#include "tink/keyset_handle.h"

namespace genc {
namespace interop {
namespace confidential_computing {
namespace {

constexpr char kLocalhostTokenUrl[] = "http://localhost/v1/token";
constexpr char kLauncherSocketPath[] = "/run/container_launcher/teeserver.sock";
constexpr char kWellKnownFileURI[] =
    "https://confidentialcomputing.googleapis.com/.well-known/openid-configuration";
constexpr char kJwksUriFieldName[] = "jwks_uri";

size_t WriteCallback(
    void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t totalSize = size * nmemb;
  output->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}

struct CurlClient {
  static absl::StatusOr<std::unique_ptr<CurlClient>> Create() {
    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
      return absl::InternalError(
          "Unable to init CURL.");
    }
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    return absl::WrapUnique<CurlClient>(new CurlClient(curl));
  }

  absl::StatusOr<std::string> GetFromUrl(const std::string& url) {
    curl_easy_setopt(curl_, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1);
    return CallInternal(url);
  }

  absl::StatusOr<std::string> GetFromUrlAndSocket(
      const std::string& url, const std::string& socket_path) {
    curl_easy_setopt(curl_, CURLOPT_UNIX_SOCKET_PATH, socket_path.c_str());
    return GetFromUrl(url);
  }

  ~CurlClient() { curl_easy_cleanup(curl_); }

 protected:
  explicit CurlClient(CURL* curl_arg) : curl_(curl_arg) {}

  absl::StatusOr<std::string> CallInternal(const std::string& url) {
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    std::string response;
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);
    CURLcode curl_code = curl_easy_perform(curl_);
    if (curl_code != CURLE_OK) {
      return absl::InternalError(absl::StrCat(
          "Received an error from \"", url, "\": \"",
          curl_easy_strerror(curl_code), "\"."));
    }
    return response;
  }

 private:
  CURL* const curl_;
};

}  // namespace

absl::StatusOr<std::string> GetAttestationToken() {
  return GENC_TRY(CurlClient::Create())->GetFromUrlAndSocket(
      kLocalhostTokenUrl, kLauncherSocketPath);
}

absl::StatusOr<crypto::tink::VerifiedJwt> DecodeAttestationToken(
    const std::string& token) {
  const std::string well_known_payload =
      GENC_TRY(GENC_TRY(CurlClient::Create())->GetFromUrl(kWellKnownFileURI));
  auto well_known_json =
      nlohmann::json::parse(well_known_payload, nullptr, false);
  if (well_known_json.is_discarded()) {
    return absl::InternalError(absl::StrCat(
        "Couldn't parse the well-known payload: ", well_known_payload));
  }
  const std::string jwks_uri = well_known_json[kJwksUriFieldName];
  const std::string jwks_payload =
      GENC_TRY(GENC_TRY(CurlClient::Create())->GetFromUrl(jwks_uri));
  std::unique_ptr<crypto::tink::KeysetHandle> keyset_handle =
      GENC_TRY(crypto::tink::JwkSetToPublicKeysetHandle(jwks_payload));
  crypto::tink::JwtValidator validator =
      GENC_TRY(crypto::tink::JwtValidatorBuilder()
                   .IgnoreAudiences()
                   .IgnoreIssuer()
                   .IgnoreTypeHeader()
                   .Build());
  GENC_TRY(crypto::tink::JwtSignatureRegister());
  std::unique_ptr<crypto::tink::JwtPublicKeyVerify> jwt_verifier =
      GENC_TRY(keyset_handle->GetPrimitive<crypto::tink::JwtPublicKeyVerify>(
          crypto::tink::ConfigGlobalRegistry()));
  crypto::tink::VerifiedJwt verified_token =
      GENC_TRY(jwt_verifier->VerifyAndDecode(token, validator));
  return verified_token;
}

namespace {

class AttestationProviderImpl : public AttestationProvider {
 public:
  absl::StatusOr<::oak::session::v1::EndorsedEvidence>
      GetEndorsedEvidence() override {
    std::string token = GENC_TRY(GetAttestationToken());
    crypto::tink::VerifiedJwt verified_token =
        GENC_TRY(DecodeAttestationToken(token));

    // TODO(b/333410413): Implement the rest of this, as per the below, and
    // package it in Oak's attestation data structure.
    // https://cloud.google.com/confidential-computing/confidential-space/docs/connect-external-resources#unencrypted

    return absl::UnimplementedError(absl::StrCat(
        "GetEndorsedEvidence has not been fully implemented yet, but here's "
        "the raw token:\n", token, "\nand here's its JSON payload:\n",
        GENC_TRY(verified_token.GetJsonPayload())));
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
