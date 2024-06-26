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

package org.genc.runtime;

final class OwnedValueId {
  static {
    System.loadLibrary("owned_value_id_jni");
  }

  public static OwnedValueId create(long nativeHandle) {
    return new OwnedValueId(nativeHandle);
  }

  public ValueId ref() {
    return ValueId.create(ref(this.nativeHandle));
  }

  private OwnedValueId(long nativeHandle) {
    this.nativeHandle = nativeHandle;
  }

  private long nativeHandle;

  private static native long ref(long nativeHandle);
}
