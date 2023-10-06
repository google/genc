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

#include "generative_computing/c/runtime/c_api.h"

#include <stdio.h>
#include <stdlib.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <utility>

#include "absl/status/statusor.h"
#include "generative_computing/c/runtime/c_api_internal.h"  // IWYU pragma: keep
#include "generative_computing/c/runtime/gc_buffer.h"
#include "generative_computing/cc/runtime/executor.h"
#include "generative_computing/cc/runtime/executor_stacks.h"
#include "generative_computing/proto/v0/computation.pb.h"

// TODO(b/295041820): Generalize this to all protobufs by fixing copybara issues
void MessageToBuffer(const generative_computing::v0::Value& in,
                     GC_Buffer* out) {
  if (out->data != nullptr) {
    return;
  }
  const size_t proto_size = in.ByteSizeLong();
  void* buf = malloc(proto_size);
  if (buf == nullptr) {
    return;
  }
  if (!in.SerializeWithCachedSizesToArray(static_cast<uint8_t*>(buf))) {
    free(buf);
    return;
  }
  out->data = buf;
  out->length = proto_size;
  out->data_deallocator = [](void* data, size_t length) { free(data); };
}

void BufferToMessage(const GC_Buffer* in,
                     generative_computing::v0::Value* out) {
  if (in == nullptr || !out->ParseFromArray(in->data, in->length)) {
    return;
  }
}

extern "C" {

uint64_t GC_GetValue(GC_OwnedValueId* owned_value_id) {
  return owned_value_id->owned_value_id.ref();
}

GC_Executor* GC_create() {
  absl::StatusOr<std::shared_ptr<generative_computing::Executor>> e =
      generative_computing::CreateDefaultLocalExecutor();
  if (!e.ok()) {
    return nullptr;
  }
  GC_Executor* executor = new GC_Executor(e.value());
  return executor;
}

GC_OwnedValueId* GC_create_value(GC_Executor* executor, GC_Buffer* value) {
  generative_computing::v0::Value value_proto;
  BufferToMessage(value, &value_proto);
  auto res = executor->executor->CreateValue(value_proto);
  if (!res.ok()) {
    return nullptr;
  }
  GC_OwnedValueId* ret = new GC_OwnedValueId(std::move(res.value()));
  return ret;
}

GC_OwnedValueId* GC_create_call(GC_Executor* executor, uint64_t function,
                                uint64_t arg) {
  auto res = executor->executor->CreateCall(function, arg);
  if (!res.ok()) {
    return nullptr;
  }
  GC_OwnedValueId* ret = new GC_OwnedValueId(std::move(res.value()));
  return ret;
}

void GC_materialize(GC_Executor* executor, uint64_t value, GC_Buffer* out) {
  generative_computing::v0::Value value_proto;
  auto res = executor->executor->Materialize(value, &value_proto);
  if (!res.ok()) {
    return;
  }
  MessageToBuffer(value_proto, out);
}

void GC_dispose(GC_Executor* executor, uint64_t value) {
  auto res = executor->executor->Dispose(value);
  if (!res.ok()) {
    return;
  }
}

}  // end extern "C"
