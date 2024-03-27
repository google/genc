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

#include "generative_computing/cc/types/inference.h"

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "generative_computing/cc/runtime/status_macros.h"
#include "generative_computing/cc/types/checking.h"
#include "generative_computing/proto/v0/computation.pb.h"

namespace generative_computing {
namespace {

absl::Status SetScalarType(v0::Value* value_pb, v0::ScalarType scalar_type) {
  if (!value_pb->has_type()) {
    value_pb->mutable_type()->set_scalar(scalar_type);
    return absl::OkStatus();
  }
  v0::Type type_pb;
  type_pb.set_scalar(scalar_type);
  return CheckEqual(value_pb->type(), type_pb);
}

absl::Status CreateStructType(v0::Struct* struct_pb, v0::StructType* type_pb,
                              const InferenceOptions* options_or_null) {
  for (int i = 0; i < struct_pb->element_size(); ++i) {
    v0::Value* val_element_pb = struct_pb->mutable_element(i);
    v0::Type* type_element_pb = type_pb->add_element();
    GENC_TRY(InferTypes(val_element_pb, options_or_null));
    type_element_pb->CopyFrom(val_element_pb->type());
  }
  return absl::OkStatus();
}

}  // namespace

absl::Status InferTypes(v0::Value* value_pb,
                        const InferenceOptions* options_or_null) {
  if (value_pb == nullptr) {
    return absl::InvalidArgumentError("Null value.");
  }
  if (value_pb->has_str()) {
    return SetScalarType(value_pb, v0::SCALAR_TYPE_STRING);
  }
  if (value_pb->has_int_32()) {
    return SetScalarType(value_pb, v0::SCALAR_TYPE_INT32);
  }
  if (value_pb->has_boolean()) {
    return SetScalarType(value_pb, v0::SCALAR_TYPE_BOOL);
  }
  if (value_pb->has_struct_()) {
    if (!value_pb->has_type()) {
      return CreateStructType(value_pb->mutable_struct_(),
                              value_pb->mutable_type()->mutable_struct_(),
                              options_or_null);
    }
    v0::Type type_pb;
    GENC_TRY(CreateStructType(value_pb->mutable_struct_(),
                              type_pb.mutable_struct_(), options_or_null));
    return CheckEqual(value_pb->type(), type_pb);
  }
  if (value_pb->has_selection()) {
    GENC_TRY(InferTypes(value_pb->mutable_selection()->mutable_source(),
                        options_or_null));
    if (!value_pb->selection().source().type().has_struct_()) {
      return absl::InvalidArgumentError(
          absl::StrCat("Selecting from a non-struct type ",
                       value_pb->selection().source().type().DebugString()));
    }
    if (value_pb->selection().index() < 0) {
      return absl::InvalidArgumentError("Negative selection index.");
    }
    if (value_pb->selection().index() >=
        value_pb->selection().source().type().struct_().element_size()) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Selection index ", value_pb->selection().index(),
          " out of range for a struct of size ",
          value_pb->selection().source().type().struct_().element_size(), "."));
    }
    const v0::Type& selected_type =
        value_pb->selection().source().type().struct_().element(
            value_pb->selection().index());
    if (!value_pb->has_type()) {
      value_pb->mutable_type()->CopyFrom(selected_type);
      return absl::OkStatus();
    } else {
      return CheckEqual(value_pb->type(), selected_type);
    }
  }
  return absl::UnimplementedError("Not implemented.");
}

}  // namespace generative_computing
