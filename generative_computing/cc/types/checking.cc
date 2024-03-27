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

#include "generative_computing/cc/types/checking.h"

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace {

absl::Status CheckScalarEqual(v0::ScalarType x, v0::ScalarType y) {
  if (x == y) {
    return absl::OkStatus();
  }
  return absl::InvalidArgumentError(
      absl::StrCat("Scalar ", v0::ScalarType_Name(x),
                   " vs. a different scalar ", v0::ScalarType_Name(y), "."));
}

absl::Status CheckStructEqual(v0::StructType x, v0::StructType y) {
  if (x.element_size() != y.element_size()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Inequal number of struct elements ", x.element_size(),
                     " vs. ", y.element_size(), "."));
  }
  for (int i = 0; i < x.element_size(); ++i) {
    absl::Status status = CheckEqual(x.element(i), y.element(i));
    if (!status.ok()) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Type mismatch at struct element ", i, ": ", status.message()));
    }
  }
  return absl::OkStatus();
}

absl::Status CheckFunctionEqual(v0::FunctionType x, v0::FunctionType y) {
  absl::Status status = CheckEqual(x.parameter(), y.parameter());
  if (!status.ok()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Function parameter type mismatch: ", status.message()));
  }
  status = CheckEqual(x.result(), y.result());
  if (!status.ok()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Function result type mismatch: ", status.message()));
  }
  return absl::OkStatus();
}

}  // namespace

absl::Status CheckEqual(v0::Type x, v0::Type y) {
  if (x.has_scalar()) {
    if (!y.has_scalar()) {
      return absl::InvalidArgumentError(
          absl::StrCat("Scalar ", v0::ScalarType_Name(x.scalar()),
                       " vs. non-scalar ", y.DebugString(), "."));
    }
    return CheckScalarEqual(x.scalar(), y.scalar());
  }
  if (x.has_struct_()) {
    if (!y.has_struct_()) {
      return absl::InvalidArgumentError(absl::StrCat("Struct ", x.DebugString(),
                                                     " vs. non-struct ",
                                                     y.DebugString(), "."));
    }
    return CheckStructEqual(x.struct_(), y.struct_());
  }
  if (x.has_function()) {
    if (!y.has_function()) {
      return absl::InvalidArgumentError(
          absl::StrCat("Function ", x.DebugString(), " vs. non-function ",
                       y.DebugString(), "."));
    }
    return CheckFunctionEqual(x.function(), y.function());
  }
  return absl::UnimplementedError("Not implemented.");
}

}  // namespace generative_computing
