/* Copyright 2023, The Generative Computing Authors.

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

#include "generative_computing/cc/interop/backends/openai.h"

#include <cstddef>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <curl/curl.h>
#include <curl/curl.h>
#include "generative_computing/cc/intrinsics/model_inference.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

namespace {

constexpr char kOpenAIModelUri[] = "/openai/chatgpt";

// Callback fn to write the response.
size_t WriteCallback(void* contents, size_t size, size_t nmemb,
                     std::string* output) {
  size_t totalSize = size * nmemb;
  output->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}
}  // namespace

absl::StatusOr<v0::Value> OpenAI::Call(const v0::Value& input) {
  std::string json_request;
  std::string api_key;
  std::string endpoint;

  // Unpack the keyword arguments.
  for (const v0::Value& arg : input.struct_().element()) {
    if (arg.label() == "api_key") {
      api_key = arg.str();
    } else if (arg.label() == "endpoint") {
      endpoint = arg.str();
    } else if (arg.label() == "json_request") {
      json_request = arg.str();
    }
  }

  CURL* curl = curl_easy_init();

  if (curl == nullptr) return absl::InternalError("Unable to init CURL");

  curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());

  struct curl_slist* headers = NULL;
  headers =
      curl_slist_append(headers, ("Authorization: Bearer " + api_key).c_str());
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_request.c_str());

  // Set the callback function to put the curl response into a buffer.
  v0::Value response;
  std::string* response_json = response.mutable_str();
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_json);

  // Send the request
  CURLcode curl_code = curl_easy_perform(curl);

  // Error out if call fails
  if (curl_code != CURLE_OK) {
    return absl::InternalError(curl_easy_strerror(curl_code));
  }

  // Cleanup, free resource and return.
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return response;
}

absl::StatusOr<v0::Value> OpenAI::CreateRequest(std::string api_key,
                                                std::string endpoint,
                                                std::string json_request) {
  v0::Value request;
  v0::Struct* args = request.mutable_struct_();
  v0::Value* arg_api_key = args->add_element();
  arg_api_key->set_label("api_key");
  arg_api_key->set_str(api_key);

  v0::Value* arg_endpoint = args->add_element();
  arg_endpoint->set_label("endpoint");
  arg_endpoint->set_str(endpoint);

  v0::Value* arg_json_request = args->add_element();
  arg_json_request->set_label("json_request");
  arg_json_request->set_str(json_request);
  return request;
}

absl::Status OpenAI::SetInferenceMap(
    intrinsics::ModelInference::InferenceMap& inference_map) {
  inference_map[kOpenAIModelUri] = [](v0::Value arg) { return Call(arg); };

  return absl::OkStatus();
}
}  // namespace generative_computing
