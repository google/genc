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

#ifndef GENERATIVE_COMPUTING_CC_RUNTIME_CARDINALITIES_H_
#define GENERATIVE_COMPUTING_CC_RUNTIME_CARDINALITIES_H_

#include <string>
#include <string_view>

#include "absl/container/btree_map.h"
#include "absl/status/statusor.h"

namespace generative_computing {

// `btree_map` is used to provide ordering.
//
// This ensures that simple string-joining of the entries in this map will
// produce a specific value for a given set of cardinalities independent of
// ordering.
using CardinalityMap = absl::btree_map<std::string, int>;
const std::string_view kClientsUri = "clients";
const std::string_view kServerUri = "server";

// Returns the number of clients specified by the provided `cardinalities`.
absl::StatusOr<int> NumClientsFromCardinalities(
    const CardinalityMap& cardinalities);

}  // namespace generative_computing

#endif  // GENERATIVE_COMPUTING_CC_RUNTIME_CARDINALITIES_H_
