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

#include "generative_computing/c/runtime/gc_buffer.h"

#include <stdio.h>
#include <stdlib.h>

#include <cstddef>
#include <cstring>

extern "C" {

GC_Buffer* GC_NewBuffer() { return new GC_Buffer{nullptr, 0, nullptr}; }

GC_Buffer* GC_NewBufferFromString(const void* proto, size_t proto_len) {
  void* copy = malloc(proto_len);
  std::memcpy(copy, proto, proto_len);

  GC_Buffer* buf = new GC_Buffer;
  buf->data = copy;
  buf->length = proto_len;
  buf->data_deallocator = [](void* data, size_t length) { free(data); };
  return buf;
}

void GC_DeleteBuffer(GC_Buffer* buffer) {
  if (buffer == nullptr) return;
  if (buffer->data_deallocator != nullptr) {
    (*buffer->data_deallocator)(const_cast<void*>(buffer->data),
                                buffer->length);
  }
  delete buffer;
}

GC_Buffer GC_GetBuffer(GC_Buffer* buffer) { return *buffer; }

}  // end extern "C"
