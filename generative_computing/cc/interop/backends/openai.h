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

#ifndef GENERATIVE_COMPUTING_CC_INTEROP_BACKENDS_OPENAI_H_
#define GENERATIVE_COMPUTING_CC_INTEROP_BACKENDS_OPENAI_H_

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "generative_computing/cc/intrinsics/model_inference.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {

// Backend for calling https://platform.openai.com/docs/api-reference
class OpenAI final {
 public:
  ~OpenAI() = default;

  // Not copyable or movable.
  OpenAI(const OpenAI&) = delete;
  OpenAI& operator=(const OpenAI&) = delete;

  // To GenC Value, which forms a request to Call.
  // See https://platform.openai.com/docs/api-reference for JSON format.
  static absl::StatusOr<v0::Value> CreateRequest(
      std::string api_key, std::string endpoint,
      std::string json_request);

  static absl::StatusOr<v0::Value> Call(const v0::Value& input);

  // Sets the inference map to process model calls to OpenAI.
  static absl::Status SetInferenceMap(
      intrinsics::ModelInference::InferenceMap& inference_map);

 private:
  // Do not hold states in this class.
  OpenAI() = default;
};
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_INTEROP_BACKENDS_OPENAI_H_
