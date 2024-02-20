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

package src.java.org.generativecomputing.interop.backends.mediapipe;

import android.content.Context;
import com.google.common.flogger.FluentLogger;
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
  private static final String KEY_MAX_SEQUENCE_LENGTH = "max_sequence_length";
  private static final String KEY_TOP_K = "top_k";
  private static final String KEY_TEMPERATURE = "temperature";
  private static final String KEY_RANDOM_SEED = "random_seed";

  private final Context context;

  public LlmInferenceClient(Context context) {
    this.context = context;
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
    // TODO(b/325824043): Enable LLMInference calls in OSS post launch.
    return response;
  }
}
