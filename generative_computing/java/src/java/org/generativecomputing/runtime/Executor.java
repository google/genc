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

package org.generativecomputing;

final class Executor {
  public static Executor create(long executorHandle) {
    return new Executor(executorHandle);
  }

  public OwnedValueId createValue(Value value) {
    return OwnedValueId.create(createValue(this.nativeHandle, value.toByteArray()));
  }

  public OwnedValueId createCall(ValueId function, ValueId arg) {
    return OwnedValueId.create(createCall(this.nativeHandle, function.getValue(), arg.getValue()));
  }

  public Value materialize(ValueId value) {
    try {
      return Value.parseFrom(materialize(this.nativeHandle, value.getValue()));
    } catch (Exception e) {
      return Value.newBuilder().setStr("").build();
    }
  }

  public void dispose(ValueId value) {
    dispose(this.nativeHandle, value.getValue());
  }

  private Executor(long nativeHandle) {
    this.nativeHandle = nativeHandle;
  }

  private final long nativeHandle;

  private static native long createValue(long nativeHandle, byte[] value);

  private static native long createCall(long nativeHandle, long function, long arg);

  private static native byte[] materialize(long nativeHandle, long value);

  private static native void dispose(long nativeHandle, long value);

  static {
    // libapp.so needs to be linked or app will crash.
    System.loadLibrary("app");
  }
}
