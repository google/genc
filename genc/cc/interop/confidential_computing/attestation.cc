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

#include <chrono>  // NOLINT
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/escaping.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/strings/substitute.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include "genc/cc/interop/confidential_computing/attestation.h"
#include "genc/cc/runtime/status_macros.h"
#include <nlohmann/json.hpp>
#include "proto/attestation/evidence.pb.h"
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
constexpr char kAttestationJsonRequestTemplate[] = R"json(
{
  "audience": "$0",
  "token_type": "OIDC",
  "nonces": [
    "$1"
  ]
})json";
constexpr char kAudience[] = "GenC";
constexpr char kIssuer[] = "https://confidentialcomputing.googleapis.com";
constexpr char kJwtNonceAttributeName[] = "eat_nonce";
constexpr char kSubmodsAttributeName[] = "submods";
constexpr char kContainerAttributeName[] = "container";
constexpr char kContainerImageReferenceAttributeName[] = "image_reference";
constexpr char kContainerImageDigestAttributeName[] = "image_digest";
constexpr char kOsAttributeName[] = "swname";
constexpr char kOsAttributeValue[] = "CONFIDENTIAL_SPACE";

size_t WriteCallback(
    void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t totalSize = size * nmemb;
  output->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}

struct CurlClient {
  static absl::StatusOr<std::unique_ptr<CurlClient>> Create(
      bool debug = false) {
    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
      return absl::InternalError(
          "Unable to init CURL.");
    }
    if (debug) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    }
    return absl::WrapUnique<CurlClient>(new CurlClient(curl));
  }

  absl::StatusOr<std::string> GetFromUrl(const std::string& url) {
    curl_easy_setopt(curl_, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1);
    return CallInternal(url);
  }

  absl::StatusOr<std::string> PostJsonToUrl(
      const std::string& url,
      const std::string& json_request) {
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, json_request.c_str());
    absl::StatusOr<std::string> result = CallInternal(url);
    curl_slist_free_all(headers);
    return result;
  }

  absl::StatusOr<std::string> GetFromUrlAndSocket(
      const std::string& url,
      const std::string& socket_path) {
    curl_easy_setopt(curl_, CURLOPT_UNIX_SOCKET_PATH, socket_path.c_str());
    return GetFromUrl(url);
  }

  absl::StatusOr<std::string> PostJsonToUrlAndSocket(
      const std::string& url,
      const std::string& socket_path,
      const std::string& json_request) {
    curl_easy_setopt(curl_, CURLOPT_UNIX_SOCKET_PATH, socket_path.c_str());
    return PostJsonToUrl(url, json_request);
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

absl::StatusOr<std::string> GetStringAttributeFromJson(
    const nlohmann::json& json, const std::vector<absl::string_view>& path) {
  if (path.empty()) {
    return absl::InvalidArgumentError("Empty path.");
  }
  int i = 0;
  auto it = json.find(path[i]);
  while (true) {
    if (it == json.end()) {
      return absl::InternalError(absl::StrCat(
          "Missing attribute \"", path[i], "\"."));
    }
    if (i >= (path.size() - 1)) {
      break;
    }
    ++i;
    it = json.find(path[i]);
  }
  return it->get<std::string>();
}

}  // namespace

absl::StatusOr<std::string> GetAttestationToken() {
  return GENC_TRY(CurlClient::Create())->GetFromUrlAndSocket(
      kLocalhostTokenUrl, kLauncherSocketPath);
}

absl::StatusOr<std::string> GetAttestationToken(
    const std::string& audience, const std::string& nonce) {
  std::string json_request = absl::Substitute(
      kAttestationJsonRequestTemplate, audience, nonce);
  return GENC_TRY(CurlClient::Create())->PostJsonToUrlAndSocket(
      kLocalhostTokenUrl, kLauncherSocketPath, json_request);
}

absl::StatusOr<crypto::tink::VerifiedJwt> DecodeAttestationToken(
    const std::string& token,
    const std::string& audience,
    const std::string& issuer) {
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
  absl::StatusOr<std::unique_ptr<crypto::tink::KeysetHandle>> keyset_handle =
      crypto::tink::JwkSetToPublicKeysetHandle(jwks_payload);
  if (!keyset_handle.ok()) {
    return absl::InternalError(absl::StrCat(
        "JwkSetToPublicKeysetHandle failed with errpr:\n",
        keyset_handle.status().ToString(),
        "\non payload:\n",
        jwks_payload, "\n"));
  }
  crypto::tink::JwtValidatorBuilder builder;
  if (!audience.empty()) {
    builder.ExpectAudience(audience);
  } else {
    builder.IgnoreAudiences();
  }
  if (!issuer.empty()) {
    builder.ExpectIssuer(issuer);
  } else {
    builder.IgnoreIssuer();
  }
  builder.IgnoreTypeHeader();
  crypto::tink::JwtValidator validator = GENC_TRY(builder.Build());
  GENC_TRY(crypto::tink::JwtSignatureRegister());
  std::unique_ptr<crypto::tink::JwtPublicKeyVerify> jwt_verifier = GENC_TRY(
      keyset_handle.value()->GetPrimitive<crypto::tink::JwtPublicKeyVerify>(
          crypto::tink::ConfigGlobalRegistry()));
  absl::StatusOr<crypto::tink::VerifiedJwt> verified_token =
      jwt_verifier->VerifyAndDecode(token, validator);
  if (!verified_token.ok()) {
    return absl::InternalError(absl::StrCat(
        "VerifyAndDecode failed with error:\n",
        verified_token.status().ToString(),
        "\non token:\n",
        token, "\n"));
  }
  return verified_token;
}

namespace {

class AttestationProviderImpl : public AttestationProvider {
 public:
  explicit AttestationProviderImpl(bool debug) : debug_(debug) {}

  absl::StatusOr<::oak::session::v1::EndorsedEvidence>
      GetEndorsedEvidence(const std::string& serialized_public_key) override {
    // Supply the key as a nonce.
    const std::string nonce = absl::Base64Escape(serialized_public_key);
    absl::StatusOr<std::string> token = GetAttestationToken(kAudience, nonce);
    if (!token.ok()) {
      return absl::InternalError(absl::StrCat(
          "AttestationProvider couldn't get attestation token for audience \"",
          kAudience, "\" and nonce \"", nonce, "\": ",
          token.status().ToString()));
    }
    if (debug_) {
      absl::StatusOr<crypto::tink::VerifiedJwt> verified_token =
          DecodeAttestationToken(token.value(), kAudience, kIssuer);
      if (verified_token.ok()) {
        std::cout << "Attestation token payload:\n"
                  << verified_token->GetJsonPayload() << "\n";
      } else {
        std::cout << "Attestation token seems invalid:\n"
                  << verified_token.status().ToString() << "\n";
      }
    }
    ::oak::session::v1::EndorsedEvidence evidence;
    auto root_layer = evidence.mutable_evidence()->mutable_root_layer();
    // TODO(b/333410413): Potentially extract correct info from the JWT token
    // and plug it into the root layer instead of the JWT token.
    root_layer->set_platform(::oak::attestation::v1::TEE_PLATFORM_UNSPECIFIED);
    root_layer->set_remote_attestation_report(token.value());
    auto app_keys = evidence.mutable_evidence()->mutable_application_keys();
    // The client-side verifier expects the JWT token, and will unpack it the
    // same way as we do above.
    app_keys->set_encryption_public_key_certificate(token.value());
    if (debug_) {
        std::cout << "AttestationProvider returning endorsed evidence:\n"
                  << evidence.DebugString() << "\n";
    }
    return evidence;
  }

  virtual ~AttestationProviderImpl() {};

 private:
  const bool debug_;
};

class AttestationVerifierImpl : public AttestationVerifier {
 public:
  explicit AttestationVerifierImpl(
      WorkloadProvenance workload_provenance, bool debug)
      : workload_provenance_(workload_provenance), debug_(debug) {}

  absl::StatusOr<::oak::attestation::v1::AttestationResults> Verify(
      std::chrono::time_point<std::chrono::system_clock> now,
      const ::oak::attestation::v1::Evidence& evidence,
      const ::oak::attestation::v1::Endorsements& endorsements) const override {
    // The server-side inserts JWT token here, to be unpacked on the client.
    const std::string& token =
        evidence.application_keys().encryption_public_key_certificate();
    crypto::tink::VerifiedJwt verified_token =
        GENC_TRY(DecodeAttestationToken(token, kAudience, kIssuer));
    const std::string token_json_string =
        GENC_TRY(verified_token.GetJsonPayload());
    if (debug_) {
      std::cout << "Attestation token payload:\n" << token_json_string << "\n";
    }
    nlohmann::json token_json =
        nlohmann::json::parse(token_json_string, nullptr, false);
    if (token_json.is_discarded()) {
      return absl::InternalError(absl::StrCat(
        "Couldn't parse the token JSON string: ", token_json_string));
    }
    const std::string os_name = GENC_TRY(GetStringAttributeFromJson(
        token_json, {kOsAttributeName}));
    if (os_name != kOsAttributeValue) {
      return absl::InternalError(absl::StrCat(
          "OS name mismatch: ", os_name, " vs. ", kOsAttributeValue));
    }
    if (!workload_provenance_.container_image_reference.empty()) {
      const std::string& image_reference = GENC_TRY(GetStringAttributeFromJson(
        token_json, {
          kSubmodsAttributeName,
          kContainerAttributeName,
          kContainerImageReferenceAttributeName}));
      if (image_reference != workload_provenance_.container_image_reference) {
        return absl::InternalError(absl::StrCat(
            "Container image digest mismatch: ", image_reference,
            " vs. ", workload_provenance_.container_image_reference));
      }
    }
    if (!workload_provenance_.container_image_digest.empty()) {
      const std::string& image_digest = GENC_TRY(GetStringAttributeFromJson(
        token_json, {
          kSubmodsAttributeName,
          kContainerAttributeName,
          kContainerImageDigestAttributeName}));
      if (image_digest != workload_provenance_.container_image_digest) {
        return absl::InternalError(absl::StrCat(
            "Container image digest mismatch: ", image_digest,
            " vs. ", workload_provenance_.container_image_digest));
      }
    }
    const std::string& eat_nonce = GENC_TRY(GetStringAttributeFromJson(
        token_json, {kJwtNonceAttributeName}));
    ::oak::attestation::v1::AttestationResults attestation_results;
    attestation_results.set_status(
        ::oak::attestation::v1::AttestationResults::STATUS_SUCCESS);
    // The nonce was the key.
    std::string serialized_public_key;
    if (!absl::Base64Unescape(eat_nonce, &serialized_public_key)) {
      return absl::InternalError(absl::StrCat(
          "Couldn't convert the nonce to a serialized public key: ",
          eat_nonce));
    }
    attestation_results.set_encryption_public_key(serialized_public_key);  // NOLINT
    return attestation_results;
  }

  virtual ~AttestationVerifierImpl() {};

 private:
  const WorkloadProvenance workload_provenance_;
  const bool debug_;
};

}  // namespace

absl::StatusOr<std::shared_ptr<AttestationProvider>>
CreateAttestationProvider(bool debug) {
  return std::make_shared<AttestationProviderImpl>(debug);
}

absl::StatusOr<std::shared_ptr<AttestationVerifier>> CreateAttestationVerifier(
    WorkloadProvenance workload_provenance, bool debug) {
  return std::make_shared<AttestationVerifierImpl>(workload_provenance, debug);
}

}  // namespace confidential_computing
}  // namespace interop
}  // namespace genc
