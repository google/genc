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
#ifndef GENC_GOOGLE_CC_INTEROP_BACKENDS_LLAMACPP_H_
#define GENC_GOOGLE_CC_INTEROP_BACKENDS_LLAMACPP_H_

#include <memory>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "genc/cc/intrinsics/model_inference.h"
#include "genc/proto/v0/computation.pb.h"
#include "llama.h"

namespace genc {

class LlamaCpp {
 public:
  LlamaCpp() = default;
  ~LlamaCpp() = default;

  bool is_initialized() const {
    return (context_ != nullptr) && (model_ != nullptr);
  }

  absl::Status InitModel(absl::string_view model_path, int num_threads,
                         int max_tokens);
  absl::Status InitModel(const v0::Value& config);
  absl::StatusOr<v0::Value> CreateRequest(std::string prompt);
  absl::Status SetInferenceMap(
      intrinsics::ModelInference::InferenceMap& inference_map,
      absl::string_view model_uri);
  absl::StatusOr<v0::Value> LlamaCppCall(const v0::Value& input);

  // Disallow copy and assign.
  LlamaCpp(const LlamaCpp&) = delete;
  LlamaCpp& operator=(const LlamaCpp&) = delete;
  LlamaCpp(LlamaCpp&&) = delete;
  LlamaCpp& operator=(LlamaCpp&&) = delete;

 private:
  struct llama_model* model_;
  struct llama_context* context_;
  int max_tokens_;
};

absl::StatusOr<v0::Value> CallLlamaCpp(
    const v0::Value& config, const v0::Value& arg);

std::function<absl::StatusOr<v0::Value>(v0::Intrinsic, v0::Value)>
GetLlamaCppInferenceFn();

}  // namespace genc

#endif  // GENC_GOOGLE_CC_INTEROP_BACKENDS_LLAMACPP_H_
