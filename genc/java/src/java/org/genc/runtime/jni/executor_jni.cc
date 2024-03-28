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

#include "genc/java/src/java/org/genc/runtime/jni/executor_jni.h"

#include "genc/c/runtime/c_api.h"
#include "genc/c/runtime/gc_buffer.h"
#include "genc/cc/runtime/executor.h"
#include "genc/proto/v0/computation.pb.h"
#include "genc/proto/v0/executor.pb.h"

namespace {

// Parses proto message from jbyteArray.
template <typename M>
bool JbyteArrayToProto(JNIEnv* env, jbyteArray data, M* message) {
  jboolean is_copy;
  jbyte* bytes = env->GetByteArrayElements(data, &is_copy);
  size_t len = env->GetArrayLength(data);
  return message->ParseFromArray(bytes, len);
}

// Serializes proto message to jbyteArray.
template <typename M>
inline jbyteArray ProtoToJbyteArray(JNIEnv* env, const M& message) {
  jsize len = message.ByteSizeLong();
  jbyteArray output = env->NewByteArray(len);
  jbyte* data = env->GetByteArrayElements(output, nullptr);
  bool serialization_successful =
      message.SerializeToArray(static_cast<void*>(data), len);
  env->ReleaseByteArrayElements(output, data, 0);
  if (!serialization_successful) {
    env->DeleteLocalRef(output);
    return nullptr;
  }
  return output;
}

void GC_MaybeDeleteBuffer(GC_Buffer* buf) {
  if (buf == nullptr) return;
  GC_DeleteBuffer(buf);
}

typedef std::unique_ptr<GC_Buffer, decltype(&GC_MaybeDeleteBuffer)>
    unique_tf_buffer;

unique_tf_buffer MakeUniqueBuffer(GC_Buffer* buf) {
  return unique_tf_buffer(buf, GC_MaybeDeleteBuffer);
}

GC_Executor* requireHandle(JNIEnv* env, jlong handle) {
  static_assert(sizeof(jlong) >= sizeof(GC_Executor*),
                "Cannot package C object pointers as a Java long");
  if (handle == 0) {
    return nullptr;
  }
  return reinterpret_cast<GC_Executor*>(handle);
}

}  // namespace

JNIEXPORT jlong JNICALL Java_org_genc_Executor_createValue(
    JNIEnv* env, jclass clazz, jlong executor_handle, jbyteArray value) {
  GC_Executor* gc_executor = requireHandle(env, executor_handle);
  if (gc_executor == nullptr) {
    return 0;
  }
  unique_tf_buffer value_proto(MakeUniqueBuffer(nullptr));
  jbyte* jvalue_data = nullptr;
  if (value != nullptr) {
    size_t sz = env->GetArrayLength(value);
    if (sz > 0) {
      jvalue_data = env->GetByteArrayElements(value, nullptr);
      GC_Buffer* buf = GC_NewBufferFromString(
          static_cast<void*>(jvalue_data), sz);
      if (buf == nullptr) {
        return 0;
      }
      value_proto.reset(buf);
    }
  }
  GC_OwnedValueId* owned_value_id =
      GC_create_value(gc_executor, value_proto.get());
  return reinterpret_cast<jlong>(owned_value_id);
}

JNIEXPORT jlong JNICALL Java_org_genc_Executor_createCall(
    JNIEnv* env, jclass clazz, jlong executor_handle, jlong function,
    jlong arg) {
  GC_Executor* gc_executor = requireHandle(env, executor_handle);
  if (gc_executor == nullptr) {
    return 0;
  }
  GC_OwnedValueId* owned_value_id = GC_create_call(
      gc_executor, static_cast<uint64_t>(function), static_cast<uint64_t>(arg));
  return reinterpret_cast<jlong>(owned_value_id);
}

JNIEXPORT jbyteArray JNICALL Java_org_genc_Executor_materialize(
    JNIEnv* env, jclass clazz, jlong executor_handle, jlong value) {
  GC_Executor* gc_executor = requireHandle(env, executor_handle);
  if (gc_executor == nullptr) {
    return 0;
  }
  GC_Buffer* buf = GC_NewBuffer();
  GC_materialize(gc_executor, value, buf);
  jint ret_len = static_cast<jint>(buf->length);
  jbyteArray ret = env->NewByteArray(ret_len);
  env->SetByteArrayRegion(ret, 0, ret_len,
                          static_cast<const jbyte*>(buf->data));
  return ret;
}

JNIEXPORT void JNICALL Java_org_genc_Executor_dispose(
    JNIEnv* env, jclass clazz, jlong executor_handle, jlong value) {
  GC_Executor* gc_executor = requireHandle(env, executor_handle);
  if (gc_executor == nullptr) {
    return;
  }
  GC_dispose(gc_executor, value);
}
