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

import com.google.common.collect.ImmutableMap;
import com.google.protobuf.ExtensionRegistryLite;
import com.google.protobuf.InvalidProtocolBufferException;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import javax.annotation.Nullable;
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

  @Nullable
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
  @Nullable
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
  @Nullable
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
  @Nullable
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
  @Nullable
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
  @Nullable
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
  @Nullable
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
  @Nullable
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
  @Nullable
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
  @Nullable
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
  @Nullable
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

  // Constructs a struct from named values.
  @Nullable
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

  // Constructs a repeat loop with given number of steps.
  @Nullable
  public static Value createRepeat(int numSteps, Value bodyFn) {
    try {
      byte[] value = nativeCreateRepeat(numSteps, bodyFn.toByteArray());
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a lambda with given argName and body.
  @Nullable
  public static Value createLambda(String argName, Value body) {
    try {
      byte[] value = nativeCreateLambda(argName, body.toByteArray());
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a reference to given argName.
  @Nullable
  public static Value createReference(String argName) {
    try {
      byte[] value = nativeCreateReference(argName);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a regular expression partial match with the given pattern.
  @Nullable
  public static Value createRegexPartialMatch(String patternStr) {
    try {
      byte[] value = nativeCreateRegexPartialMatch(patternStr);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a logical not expression.
  @Nullable
  public static Value createLogicalNot() {
    try {
      byte[] value = nativeCreateLogicalNot();
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a repeated conditional chain expression.
  @Nullable
  public static Value createRepeatedConditionalChain(int numSteps, List<Value> bodyFns) {
    try {
      List<byte[]> serializedValueList = new ArrayList<>();
      for (Value value : bodyFns) {
        serializedValueList.add(value.toByteArray());
      }
      byte[] value = nativeCreateRepeatedConditionalChain(numSteps, serializedValueList);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a breakable chain expression.
  @Nullable
  public static Value createBreakableChain(List<Value> fnsList) {
    try {
      List<byte[]> serializedValueList = new ArrayList<>();
      for (Value value : fnsList) {
        serializedValueList.add(value.toByteArray());
      }
      byte[] value = nativeCreateBreakableChain(serializedValueList);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a logger expression.
  @Nullable
  public static Value createLogger() {
    try {
      byte[] value = nativeCreateLogger();
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a selection expression.
  @Nullable
  public static Value createSelection(Value source, int index) {
    try {
      byte[] value = nativeCreateSelection(source.toByteArray(), index);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a lambda for conditional.
  @Nullable
  public static Value createLambdaForConditional(
      Value condition, Value positiveBranch, Value negativeBranch) {
    try {
      byte[] value =
          nativeCreateLambdaForConditional(
              condition.toByteArray(), positiveBranch.toByteArray(), negativeBranch.toByteArray());
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a while loop.
  @Nullable
  public static Value createWhile(Value conditionFn, Value bodyFn) {
    try {
      byte[] value = nativeCreateWhile(conditionFn.toByteArray(), bodyFn.toByteArray());
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  // Constructs a model config Value proto with a map of key, value pairs specifying configuration
  // settings.
  public static Value createModelConfig(Map<String, Object> configMap) {
    Value.Builder modelConfig = Value.newBuilder(ToFromValueProto.toValueProto(configMap));
    modelConfig.setLabel("model_config");
    return modelConfig.build();
  }

  @Nullable
  public static Value createWolframAlpha(String appId) {
    try {
      byte[] value = nativeCreateWolframAlpha(appId);
      if (value == null) {
        return null;
      }
      return Value.parseFrom(value, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      return null;
    }
  }

  public static Value createGeminiModelConfigForGoogleAiStudio(
      String apiKey, String endpoint, String jsonRequestTemplate) {
    ImmutableMap<String, Object> map =
        ImmutableMap.of(
            "api_key", apiKey, "endpoint", endpoint, "json_request_template", jsonRequestTemplate);
    return createModelConfig(map);
  }

  public static Value createGeminiModelConfigForVertexAi(
      String accessToken, String endpoint, String jsonRequestTemplate) {
    ImmutableMap<String, Object> map =
        ImmutableMap.of(
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
    ImmutableMap<String, Object> map =
        ImmutableMap.of(
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
    ImmutableMap<String, Object> map =
        ImmutableMap.of(
            "api_key", apiKey, "endpoint", endpoint, "json_request_template", requestJsonTemplate);
    return createModelConfig(map);
  }

  public static Value createLlamaCppConfig(String modelPath, int numThreads, int maxTokens) {
    ImmutableMap<String, Object> map =
        ImmutableMap.of(
            "model_path", modelPath, "num_threads", numThreads, "max_tokens", maxTokens);
    return Constructor.createModelConfig(map);
  }

  public static Value createRestModelConfig(
      String endpoint, String apiKey, String jsonRequestTemplate) {
    ImmutableMap<String, Object> map =
        ImmutableMap.of(
            "endpoint", endpoint, "api_key", apiKey, "json_request_template", jsonRequestTemplate);
    return Constructor.createModelConfig(map);
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

  private static native byte[] nativeCreateWolframAlpha(String appId);

  private static native byte[] nativeCreateRepeat(int numSteps, byte[] bodyFn);

  private static native byte[] nativeCreateLambda(String argName, byte[] body);

  private static native byte[] nativeCreateReference(String argName);

  private static native byte[] nativeCreateRegexPartialMatch(String patternStr);

  private static native byte[] nativeCreateLogicalNot();

  private static native byte[] nativeCreateRepeatedConditionalChain(
      int numSteps, List<byte[]> bodyFns);

  private static native byte[] nativeCreateBreakableChain(List<byte[]> fnsList);

  private static native byte[] nativeCreateLogger();

  private static native byte[] nativeCreateSelection(byte[] source, int index);

  private static native byte[] nativeCreateLambdaForConditional(
      byte[] condition, byte[] positiveBranch, byte[] negativeBranch);

  private static native byte[] nativeCreateWhile(byte[] conditionFn, byte[] bodyFn);

  private Constructor() {}
}
