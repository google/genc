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

package src.java.org.generativecomputing.interop.backends.mediapipe;

import android.content.Context;
import com.google.common.annotations.VisibleForTesting;
import com.google.common.flogger.FluentLogger;
import com.google.mediapipe.tasks.genai.llminference.LlmInference;
import com.google.mediapipe.tasks.genai.llminference.LlmInference.LlmInferenceOptions;
import com.google.protobuf.ExtensionRegistryLite;
import com.google.protobuf.InvalidProtocolBufferException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import org.generativecomputing.Value;

/**
 * A client to connect to Mediapipe LLMInference APIs to run model inferences on on-device text
 * generator models. This client fills in the model config received from the callers and calls the
 * LLMInference API, returns the response.
 */
public final class LlmInferenceClient {
  private static final FluentLogger logger = FluentLogger.forEnclosingClass();
  private static final String KEY_MODEL_PATH = "model_path";
  private static final String KEY_MAX_TOKENS = "max_tokens";
  private static final String KEY_TOP_K = "top_k";
  private static final String KEY_TEMPERATURE = "temperature";
  private static final String KEY_RANDOM_SEED = "random_seed";

  private final Context context;
  private LlmInferenceOptions llmInferenceOptions;
  private LlmInference llmInference;

  public LlmInferenceClient(Context context) {
    this.context = context;
  }

  @SuppressWarnings("Finalize") // generative-computing:google3-only
  @Override
  protected void finalize() {
    if (llmInference != null) {
      llmInference.close();
    }
  }

  private boolean hasLlmInferenceOptionsChanged(
      LlmInferenceOptions llmInferenceOptions, LlmInferenceOptions latestLlmInferenceOptions) {
    return (!llmInferenceOptions.modelPath().equals(latestLlmInferenceOptions.modelPath())
        || llmInferenceOptions.maxTokens() != latestLlmInferenceOptions.maxTokens()
        || llmInferenceOptions.topK() != latestLlmInferenceOptions.topK()
        || llmInferenceOptions.temperature() != latestLlmInferenceOptions.temperature()
        || llmInferenceOptions.randomSeed() != latestLlmInferenceOptions.randomSeed());
  }

  private LlmInference getLlmInference(LlmInferenceOptions latestLlmInferenceOptions) {
    if (llmInferenceOptions == null
        || llmInference == null
        || hasLlmInferenceOptionsChanged(llmInferenceOptions, latestLlmInferenceOptions)) {
      llmInferenceOptions = latestLlmInferenceOptions;
      llmInference = LlmInference.createFromOptions(context, llmInferenceOptions);
    }
    return llmInference;
  }

  @VisibleForTesting
  public LlmInferenceOptions.Builder getLlmInferenceOptions(Map<String, Value> configSettings) {
    LlmInferenceOptions.Builder llmInferenceOptionsBuilder = LlmInferenceOptions.builder();
    if (configSettings.containsKey(KEY_MODEL_PATH)) {
      llmInferenceOptionsBuilder.setModelPath(configSettings.get(KEY_MODEL_PATH).getStr());
    }
    if (configSettings.containsKey(KEY_MAX_TOKENS)) {
      llmInferenceOptionsBuilder.setMaxTokens(
          configSettings.get(KEY_MAX_TOKENS).getInt32());
    }
    if (configSettings.containsKey(KEY_TOP_K)) {
      llmInferenceOptionsBuilder.setTopK(configSettings.get(KEY_TOP_K).getInt32());
    }
    if (configSettings.containsKey(KEY_TEMPERATURE)) {
      llmInferenceOptionsBuilder.setTemperature(configSettings.get(KEY_TEMPERATURE).getFloat32());
    }
    if (configSettings.containsKey(KEY_RANDOM_SEED)) {
      llmInferenceOptionsBuilder.setRandomSeed(configSettings.get(KEY_RANDOM_SEED).getInt32());
    }
    return llmInferenceOptionsBuilder;
  }

  private static Map<String, Value> getConfigSettings(Value config) {
    Map<String, Value> configSettings = new HashMap<>();
    for (int i = 0; i < config.getStruct().getElementCount(); i++) {
      configSettings.put(
          config.getStruct().getElement(i).getLabel(), config.getStruct().getElement(i));
    }
    return configSettings;
  }

  private boolean isValidConfigSettings(Map<String, Value> configSettings, String modelUri) {
    if ((!configSettings.containsKey(KEY_MODEL_PATH)
        || configSettings.get(KEY_MODEL_PATH).getStr().isEmpty())) {
      logger.atSevere().log(
          "For model uri: %s, required config settings were not provided. Expected 'model_path'."
              + " Found: %s",
          modelUri, Arrays.asList(configSettings));
      return false;
    }
    return true;
  }

  // Returns best available ExtensionRegistry.
  public static ExtensionRegistryLite getExtensionRegistry() {
    return ExtensionRegistryLite.getEmptyRegistry();
  }

  public String call(byte[] modelInferenceWithConfig, byte[] request) {
    String response = "";
    Value modelConfigProto;
    try {
      modelConfigProto = Value.parseFrom(modelInferenceWithConfig, getExtensionRegistry());
    } catch (InvalidProtocolBufferException e) {
      logger.atSevere().withCause(e).log("Could not parse Value proto: %s", e.getMessage());
      return response;
    }

    // Extract llm inference model configuration
    if (modelConfigProto.getIntrinsic().getStaticParameter().getStruct().getElementCount() < 2) {
      logger.atSevere().log(
          "Provided model inference with config is missing necessary fields. Number of fields"
              + " provided: %d. Number expected: 2 (model_uri, model_config)",
          modelConfigProto.getIntrinsic().getStaticParameter().getStruct().getElementCount());
    }
    Value modelUri = modelConfigProto.getIntrinsic().getStaticParameter().getStruct().getElement(0);
    Value config = modelConfigProto.getIntrinsic().getStaticParameter().getStruct().getElement(1);
    Map<String, Value> configSettings = getConfigSettings(config);

    if (!isValidConfigSettings(configSettings, modelUri.getStr())) {
      return response;
    }

    LlmInferenceOptions.Builder llmInferenceOptionsBuilder = getLlmInferenceOptions(configSettings);
    try {
      LlmInference llmInference = getLlmInference(llmInferenceOptionsBuilder.build());

      String requestString = new String(request);
      logger.atInfo().log(
          "For model uri: %s, calling MediaPipe LlmInference with input: %s",
          modelUri.getStr(), requestString);
      response = llmInference.generateResponse(requestString);
      logger.atInfo().log(
          "For model uri: %s, received response from MediaPipe LlmInference: %s",
          modelUri.getStr(), response);
    } catch (Exception e) {
      if (e instanceof InterruptedException) {
        Thread.currentThread().interrupt();
      }
      logger.atWarning().withCause(e).log(
          "MediaPipe LlmInference returned error: %s", e.getMessage());
    }
    return response;
  }
}
