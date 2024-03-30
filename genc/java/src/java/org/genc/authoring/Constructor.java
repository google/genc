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

package org.genc.authoring;

import com.google.protobuf.ExtensionRegistryLite;
import com.google.protobuf.InvalidProtocolBufferException;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import org.genc.Value;
import org.genc.runtime.ToFromValueProto;

/** Library for constructing different types of computations. */
public final class Constructor {
  static {
    System.loadLibrary("constructor_jni");
  }

  // Returns best available ExtensionRegistry.
  public static ExtensionRegistryLite getExtensionRegistry() {
    return ExtensionRegistryLite.getEmptyRegistry();
  }

  // @Nullable
  public static Value readComputationFromFile(String filePath) {
    try {
      InputStream stream = new FileInputStream(filePath);
      Value computation = Value.parseFrom(stream, getExtensionRegistry());
      stream.close();
      return computation;
    } catch (Exception e) {
      return null;
    }
  }

  // Creates a model computation with the given model URI.
  // @Nullable
  public static Value createModelInference(String modelUri) {
    try {
      byte[] value = nativeCreateModelInference(modelUri);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Returns a model inference proto with the given model URI and model config.
  // @Nullable
  public static Value createModelInferenceWithConfig(String modelUri, Value modelConfig) {
    try {
      byte[] value = nativeCreateModelInferenceWithConfig(modelUri, modelConfig.toByteArray());
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Creates a prompt template computation with the given template string.
  // @Nullable
  public static Value createPromptTemplate(String templateStr) {
    try {
      byte[] value = nativeCreatePromptTemplate(templateStr);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Creates an InjaTemplate.
  // @Nullable
  public static Value createInjaTemplate(String templateStr) {
    try {
      byte[] value = nativeCreateInjaTemplate(templateStr);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Creates a fallback expression from a given list of functions. The first
  // successful one is the result; if failed, keep going down the list.
  // @Nullable
  public static Value createFallback(List<Value> functionList) {
    try {
      List<byte[]> serializedFunctionList = new ArrayList<byte[]>();
      for (Value value : functionList) {
        serializedFunctionList.add(value.toByteArray());
      }
      byte[] value = nativeCreateFallback(serializedFunctionList);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Creates a conditional expression. condition evaluates to a boolean, if true,
  // positive_branch will be executed, else negative_branch.
  // @Nullable
  public static Value createConditional(
      Value condition, Value positiveBranch, Value negativeBranch) {
    try {
      byte[] value =
          nativeCreateConditional(
              condition.toByteArray(), positiveBranch.toByteArray(), negativeBranch.toByteArray());
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Returns a custom function proto with the given fn URI.
  // @Nullable
  public static Value createCustomFunction(String fnUri) {
    try {
      byte[] value = nativeCreateCustomFunction(fnUri);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Creates a parallel map that applies map_fn to a all input values.
  // @Nullable
  public static Value createParallelMap(Value mapFn) {
    try {
      byte[] value = nativeCreateParallelMap(mapFn.toByteArray());
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Given a list of functions [f, g, ...] create a chain g(f(...)).
  // @Nullable
  public static Value createSerialChain(List<Value> functionList) {
    try {
      List<byte[]> serializedFunctionList = new ArrayList<byte[]>();
      for (Value value : functionList) {
        serializedFunctionList.add(value.toByteArray());
      }
      byte[] value = nativeCreateSerialChain(serializedFunctionList);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a function call.
  // @Nullable
  public static Value createCall(Value fn, Value arg) {
    try {
      byte[] value = nativeCreateCall(fn.toByteArray(), arg.toByteArray());
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a struct from named values
  // @Nullable
  public static Value createStruct(List<Value> valueList) {
    try {
      List<byte[]> serializedValueList = new ArrayList<byte[]>();
      for (Value value : valueList) {
        serializedValueList.add(value.toByteArray());
      }
      byte[] value = nativeCreateStruct(serializedValueList);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  public static Value createModelConfig(Map<String, Object> configMap) {
    Value.Builder modelConfig = Value.newBuilder(ToFromValueProto.toValueProto(configMap));
    modelConfig.setLabel("model_config");
    return modelConfig.build();
  }

  public static Value createGeminiModelConfigForGoogleAiStudio(
      String apiKey, String endpoint, String jsonRequestTemplate) {
    Map<String, Object> map =
        Map.of(
            "api_key", apiKey, "endpoint", endpoint, "json_request_template", jsonRequestTemplate);
    return createModelConfig(map);
  }

  public static Value createGeminiModelConfigForVertexAi(
      String accessToken, String endpoint, String jsonRequestTemplate) {
    Map<String, Object> map =
        Map.of(
            "access_token",
            accessToken,
            "endpoint",
            endpoint,
            "json_request_template",
            jsonRequestTemplate);
    return createModelConfig(map);
  }

  public static Value createMediaPipeLlmInferenceModelConfig(
      String modelPath, int maxTokens, int topK, float temperature, int randomSeed) {
    Map<String, Object> map =
        Map.of(
            "model_path",
            modelPath,
            "max_tokens",
            maxTokens,
            "top_k",
            topK,
            "temperature",
            temperature,
            "random_seed",
            randomSeed);
    return Constructor.createModelConfig(map);
  }

  public static Value createOpenAiModelConfig(
      String apiKey, String endpoint, String requestJsonTemplate) {
    Map<String, Object> map =
        Map.of(
            "api_key", apiKey, "endpoint", endpoint, "json_request_template", requestJsonTemplate);
    return createModelConfig(map);
  }

  private static native byte[] nativeCreateModelInference(String modelUri);

  private static native byte[] nativeCreateModelInferenceWithConfig(
      String modelUri, byte[] modelConfig);

  private static native byte[] nativeCreatePromptTemplate(String templateStr);

  private static native byte[] nativeCreateFallback(List<byte[]> functionList);

  private static native byte[] nativeCreateConditional(
      byte[] condition, byte[] positiveBranch, byte[] negativeBranch);

  private static native byte[] nativeCreateCustomFunction(String fnUri);

  private static native byte[] nativeCreateInjaTemplate(String templateStr);

  private static native byte[] nativeCreateParallelMap(byte[] mapFn);

  private static native byte[] nativeCreateSerialChain(List<byte[]> functionList);

  private static native byte[] nativeCreateStruct(List<byte[]> functionList);

  private static native byte[] nativeCreateCall(byte[] fn, byte[] arg);

  private Constructor() {}
}
