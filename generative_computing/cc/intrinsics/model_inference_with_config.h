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

#ifndef GENERATIVE_COMPUTING_CC_INTRINSICS_MODEL_INFERENCE_WITH_CONFIG_H_
#define GENERATIVE_COMPUTING_CC_INTRINSICS_MODEL_INFERENCE_WITH_CONFIG_H_

#include <functional>
#include <memory>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "generative_computing/cc/intrinsics/intrinsic_uris.h"
#include "generative_computing/cc/runtime/intrinsic_handler.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace intrinsics {

class ModelInferenceWithConfig : public InlineIntrinsicHandlerBase {
 public:
  typedef std::function<absl::StatusOr<v0::Value>(
      v0::Intrinsic intrinsic_pb, const v0::Value)>
      InferenceFn;

  typedef absl::flat_hash_map<std::string, InferenceFn> InferenceMap;

  ModelInferenceWithConfig(const InferenceMap& inference_map)
      : InlineIntrinsicHandlerBase(kModelInferenceWithConfig),
        inference_map_(inference_map) {}

  virtual ~ModelInferenceWithConfig() {}

  absl::Status CheckWellFormed(const v0::Intrinsic& intrinsic_pb) const final;
  absl::Status ExecuteCall(const v0::Intrinsic& intrinsic_pb,
                           const v0::Value& arg,
                           v0::Value* result) const final;

 private:
  const InferenceMap inference_map_;
};

}  // namespace intrinsics
}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_INTRINSICS_MODEL_INFERENCE_WITH_CONFIG_H_
