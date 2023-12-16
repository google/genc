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

#include "generative_computing/cc/runtime/cardinalities.h"

#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace generative_computing {

absl::StatusOr<int> NumClientsFromCardinalities(
    const CardinalityMap& cardinalities) {
  auto entry_iter = cardinalities.find(kClientsUri);
  if (entry_iter == cardinalities.end()) {
    return absl::NotFoundError(
        "No cardinality provided for `clients` placement.");
  }
  return entry_iter->second;
}

}  // namespace generative_computing
