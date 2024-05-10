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
#include "genc/cc/interop/networking/curl_based_http_client.h"
#include "genc/cc/interop/networking/http_client_interface.h"

namespace genc {
namespace interop {
namespace networking {
namespace {

size_t WriteCallback(
    void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t totalSize = size * nmemb;
  output->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}

class CurlBasedHttpClient : public HttpClientInterface {
 public:
  static absl::StatusOr<std::unique_ptr<CurlBasedHttpClient>> Create(
      bool debug) {
    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
      return absl::InternalError(
          "Unable to init CURL.");
    }
    if (debug) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    }
    return absl::WrapUnique<CurlBasedHttpClient>(new CurlBasedHttpClient(curl));
  }

  absl::StatusOr<std::string> GetFromUrl(
      const std::string& url) override {
    curl_easy_setopt(curl_, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1);
    return CallInternal(url);
  }

  absl::StatusOr<std::string> PostJsonToUrl(
      const std::string& url,
      const std::string& json_request) override {
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
      const std::string& socket_path) override {
    curl_easy_setopt(curl_, CURLOPT_UNIX_SOCKET_PATH, socket_path.c_str());
    return GetFromUrl(url);
  }

  absl::StatusOr<std::string> PostJsonToUrlAndSocket(
      const std::string& url,
      const std::string& socket_path,
      const std::string& json_request) override {
    curl_easy_setopt(curl_, CURLOPT_UNIX_SOCKET_PATH, socket_path.c_str());
    return PostJsonToUrl(url, json_request);
  }

  ~CurlBasedHttpClient() override { curl_easy_cleanup(curl_); }

 protected:
  explicit CurlBasedHttpClient(CURL* curl_arg) : curl_(curl_arg) {}

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

absl::StatusOr<std::shared_ptr<HttpClientInterface>> CreateCurlBasedHttpClient(
    bool debug) {
  return CurlBasedHttpClient::Create(debug);
}

}  // namespace networking
}  // namespace interop
}  // namespace genc

