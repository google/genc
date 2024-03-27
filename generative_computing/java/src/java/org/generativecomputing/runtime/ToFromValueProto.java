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

import java.util.List;
import java.util.Map;

/** Util functions to convert to/from 'Value' proto. */
public final class ToFromValueProto {
  public static Value toValueProto(Object obj) {
    Value.Builder valueBuilder = Value.newBuilder();
    if (obj instanceof Integer) {
      valueBuilder.setInt32(((Integer) obj).intValue());
    } else if (obj instanceof String) {
      valueBuilder.setStr((String) obj);
    } else if (obj instanceof Float) {
      valueBuilder.setFloat32(((Float) obj).floatValue());
    } else if (obj instanceof Boolean) {
      valueBuilder.setBoolean(((Boolean) obj).booleanValue());
    } else if (obj instanceof List<?>) {
      List<?> list = (List<?>) obj;
      Struct.Builder struct = Struct.newBuilder();
      for (Object element : list) {
        struct.addElement(toValueProto(element));
      }
      valueBuilder.setStruct(struct.build());
    } else if (obj instanceof Map<?, ?>) {
      Struct.Builder struct = Struct.newBuilder();
      Map<?, ?> map = (Map<?, ?>) obj;
      for (Map.Entry<?, ?> entry : map.entrySet()) {
        Value.Builder element = Value.newBuilder(toValueProto(entry.getValue()));
        element.setLabel((String) entry.getKey());
        struct.addElement(element.build());
      }
      valueBuilder.setStruct(struct.build());
    } else {
      throw new IllegalArgumentException("Unsupported argument type: " + obj.getClass().getName());
    }
    return valueBuilder.build();
  }

  // TODO(b/306075687):Generalize to handle full Value proto.
  public static String fromValueProto(Value value) {
    return value.getStr();
  }

  private ToFromValueProto() {}
}
