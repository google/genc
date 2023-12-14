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

#include "generative_computing/cc/modules/templates/inja_status_or.h"

#include <exception>
#include <string>
#include <string_view>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

namespace inja_status_or {

absl::StatusOr<std::string> Environment::render(std::string_view input,
                                                const nlohmann::json& data) {
  try {
    return inja::Environment::render(input, data);
  } catch (const std::exception& e) {
    return absl::InternalError(absl::StrCat("Render: ", e.what()));
  } catch (...) {
    return absl::InternalError("Render: something went wrong.");
  }
}
}  // namespace inja_status_or
