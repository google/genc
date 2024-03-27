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
package src.javatests.org.generativecomputing.interop.backends.mediapipe;

import static com.google.common.truth.Truth.assertThat;

import android.content.Context;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import com.google.mediapipe.tasks.genai.llminference.LlmInference.LlmInferenceOptions;
import java.util.HashMap;
import java.util.Map;
import org.generativecomputing.Struct;
import org.generativecomputing.Value;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import src.java.org.generativecomputing.interop.backends.mediapipe.LlmInferenceClient;

/** Unit tests for LlmInferenceClient. */
@RunWith(AndroidJUnit4.class)
public final class LlmInferenceClientTest {
  private static final String KEY_MODEL_PATH = "model_path";
  private static final String KEY_MAX_TOKENS = "max_tokens";
  private static final String KEY_TOP_K = "top_k";
  private static final String KEY_TEMPERATURE = "temperature";
  private static final String KEY_RANDOM_SEED = "random_seed";
  private static final String TEST_MODEL_PATH = "/tmp/local/model/model_file.tflite";
  private static final int TEST_MAX_TOKENS = 1024;
  private static final int TEST_TOP_K = 50;
  private static final float TEST_TEMPERATURE = 0.9f;
  private static final int TEST_RANDOM_SEED = 1;

  private Context context;
  private LlmInferenceClient llmInferenceClient;

  private static Map<String, Value> getConfigSettings(Value config) {
    Map<String, Value> configSettings = new HashMap<>();
    for (int i = 0; i < config.getStruct().getElementCount(); i++) {
      configSettings.put(
          config.getStruct().getElement(i).getLabel(), config.getStruct().getElement(i));
    }
    return configSettings;
  }

  private static void verifyLlmInferenceOptions(
      LlmInferenceOptions.Builder llmInferenceOptionsBuilder) {
    LlmInferenceOptions llmInferenceOptions = llmInferenceOptionsBuilder.build();
    assertThat(llmInferenceOptions.modelPath()).isEqualTo(TEST_MODEL_PATH);
    assertThat(llmInferenceOptions.maxTokens()).isEqualTo(TEST_MAX_TOKENS);
    assertThat(llmInferenceOptions.topK()).isEqualTo(TEST_TOP_K);
    assertThat(llmInferenceOptions.temperature()).isEqualTo(TEST_TEMPERATURE);
    assertThat(llmInferenceOptions.randomSeed()).isEqualTo(TEST_RANDOM_SEED);
  }

  private static Struct getTestModelConfig() {
    return Struct.newBuilder()
        .addElement(Value.newBuilder().setLabel(KEY_MODEL_PATH).setStr(TEST_MODEL_PATH))
        .addElement(Value.newBuilder().setLabel(KEY_MAX_TOKENS).setInt32(TEST_MAX_TOKENS))
        .addElement(Value.newBuilder().setLabel(KEY_TOP_K).setInt32(TEST_TOP_K))
        .addElement(Value.newBuilder().setLabel(KEY_TEMPERATURE).setFloat32(TEST_TEMPERATURE))
        .addElement(Value.newBuilder().setLabel(KEY_RANDOM_SEED).setInt32(TEST_RANDOM_SEED))
        .build();
  }

  @Before
  public void setUp() {
    llmInferenceClient = new LlmInferenceClient(context);
  }

  @Test
  public void llmInferenceClient_correctConfigIsCreated() {
    Struct modelConfig = getTestModelConfig();
    Map<String, Value> configSettings =
        getConfigSettings(Value.newBuilder().setStruct(modelConfig).build());
    LlmInferenceOptions.Builder llmInferenceOptionsBuilder =
        llmInferenceClient.getLlmInferenceOptions(configSettings);
    verifyLlmInferenceOptions(llmInferenceOptionsBuilder);
  }
}
