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
#include "genc/cc/interop/confidential_computing/attestation.h"
#include "genc/cc/interop/networking/curl_based_http_client.h"
#include "genc/cc/interop/networking/http_client_interface.h"
#include "genc/cc/runtime/status_macros.h"
#include <nlohmann/json.hpp>
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

absl::StatusOr<std::string> GetStringAttributeFromJson(
    const nlohmann::json& json, const std::vector<absl::string_view>& path) {
  if (path.empty()) {
    return absl::InvalidArgumentError("Empty path.");
  }
  int i = 0;
  auto end = json.end();
  auto it = json.find(path[i]);
  while (true) {
    if (it == end) {
      return absl::InternalError(absl::StrCat(
          "Missing attribute \"", path[i], "\"."));
    }
    if (i >= (path.size() - 1)) {
      break;
    }
    ++i;
    end = it->end();
    it = it->find(path[i]);
  }
  return it->get<std::string>();
}

}  // namespace

absl::StatusOr<std::string> GetAttestationToken(
    std::shared_ptr<networking::HttpClientInterface> http_client) {
  if (http_client == nullptr) {
    http_client = GENC_TRY(networking::CreateCurlBasedHttpClient(false));
  }
  return http_client->GetFromUrlAndSocket(
      kLocalhostTokenUrl, kLauncherSocketPath);
}

absl::StatusOr<std::string> GetAttestationToken(
    const std::string& audience,
    const std::string& nonce,
    std::shared_ptr<networking::HttpClientInterface> http_client) {
  if (http_client == nullptr) {
    http_client = GENC_TRY(networking::CreateCurlBasedHttpClient(false));
  }
  std::string json_request = absl::Substitute(
      kAttestationJsonRequestTemplate, audience, nonce);
  return http_client->PostJsonToUrlAndSocket(
      kLocalhostTokenUrl, kLauncherSocketPath, json_request);
}

absl::StatusOr<crypto::tink::VerifiedJwt> DecodeAttestationToken(
    const std::string& token,
    const std::string& audience,
    const std::string& issuer,
    std::shared_ptr<networking::HttpClientInterface> http_client) {
  if (http_client == nullptr) {
    http_client = GENC_TRY(networking::CreateCurlBasedHttpClient(false));
  }
  const std::string well_known_payload =
      GENC_TRY(http_client->GetFromUrl(kWellKnownFileURI));
  auto well_known_json =
      nlohmann::json::parse(well_known_payload, nullptr, false);
  if (well_known_json.is_discarded()) {
    return absl::InternalError(absl::StrCat(
        "Couldn't parse the well-known payload: ", well_known_payload));
  }
  const std::string jwks_uri = well_known_json[kJwksUriFieldName];
  const std::string jwks_payload =
      GENC_TRY(http_client->GetFromUrl(jwks_uri));
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

}  // namespace confidential_computing
}  // namespace interop
}  // namespace genc
