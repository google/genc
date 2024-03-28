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

#ifndef GENC_CC_MODULES_TEMPLATES_INJA_STATUS_OR_H_
#define GENC_CC_MODULES_TEMPLATES_INJA_STATUS_OR_H_

#include <ostream>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

namespace inja_status_or {
// StatusOr wrapped inja template functions.
class Environment : public inja::Environment {
 public:
  absl::StatusOr<std::string> render(std::string_view input,
                                     const nlohmann::json& data);
};
}  // namespace inja_status_or

#endif  // GENC_CC_MODULES_TEMPLATES_INJA_STATUS_OR_H_
