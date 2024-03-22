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

package org.generativecomputing.examples.apps.gencdemo;

import androidx.annotation.Nullable;
import com.google.common.collect.ImmutableList;
import com.google.common.flogger.FluentLogger;
import com.google.protobuf.ExtensionRegistryLite;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import org.generativecomputing.Value;
import org.generativecomputing.authoring.Constructor;

/**
 * Utility for constructing different types of computations used in GenC demos. Contains default
 * values to use in demos, developers can customize these to fit their experimentations or
 * use-cases.
 */
final class Computations {
  private static final FluentLogger logger = FluentLogger.forEnclosingClass();

  // API Key to use for Gemini-Pro model inference through Google AI Studio. Follow instructions
  // to get API key and add below: https://ai.google.dev/tutorials/rest_quickstart
  public static final String GOOGLE_AI_STUDIO_API_KEY = "";

  // Gemini-Pro endpoint URL from Google AI Studio.
  public static final String GOOGLE_AI_STUDIO_GEMINI_PRO_ENDPOINT =
      "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent";

  // Default json request template for Gemini-Pro calls. Please feel free to edit this per your
  // use-case. See configuration at https://ai.google.dev/tutorials/rest_quickstart#configuration.
  public static final String GEMINI_PRO_TEXT_GENERATION_JSON_TEMPLATE =
      "{\n"
          + "  \"contents\":[{\"role\":\"user\",\"parts\":[{\"text\":\"PROMPT REPLACES"
          + " THIS\"}]}],\n"
          + "  \"safety_settings\":[{\n"
          + "    \"category\":\"HARM_CATEGORY_SEXUALLY_EXPLICIT\",\n"
          + "    \"threshold\":\"BLOCK_LOW_AND_ABOVE\"}],\n"
          + "  \"generation_config\":{\n"
          + "     \"temperature\":0.9, \"topP\":1.0, \"maxOutputTokens\":1024}\n"
          + "}";

  // Default model path for Gemma GPU model to use with Mediapipe LlmInference GenAI Tasks API.
  // Please change this if you copied the on-device model to a different path.
  public static final String MEDIAPIPE_GEMMA_GPU_ON_DEVICE_MODEL_PATH =
      "/data/local/tmp/llm/gemma-2b-it-gpu-int4.bin";

  // API Key to use for Open AI model inferences. Get API key and add below:
  // https://platform.openai.com/api-keys.
  public static final String OPEN_AI_API_KEY = "";

  // Open AI server url for chat completions API.
  public static final String OPEN_AI_CHAT_COMPLETIONS_ENDPOINT =
      "https://api.openai.com/v1/chat/completions";

  // Default json request template for Open AI calls. Please feel free to edit this per your
  // use-case. See configuration at https://platform.openai.com/docs/guides/text-generation.
  public static final String OPEN_AI_CHAT_COMPLETIONS_JSON_TEMPLATE =
      "{\n"
          + "  \"model\":\"gpt-3.5-turbo\",\n"
          + "  \"messages\":[{\"role\":\"user\",\"content\":\"PROMPT REPLACES THIS\"}]\n"
          + "}";

  // Default prompt template used in demos. Please feel free to edit this per your liking.
  public static final String PROMPT_TEMPLATE_FOR_DEMO = "Tell me about {topic}?";

  public static final String GEMINI_MODEL_URI = "/cloud/gemini";
  public static final String MEDIAPIPE_ON_DEVICE_MODEL_URI = "/device/llm_inference";
  public static final String OPEN_AI_MODEL_URI = "/openai/chatgpt";

  // Default local file path for the computation proto. Please edit this if you copy the
  // computation proto to a different path.
  public static final String COMPUTATION_FILE_PATH = "/data/local/tmp/genc_demo.pb";

  // Whether to read the computation from a local file.
  public static final Boolean IS_READ_COMPUTATION_FROM_FILE = false;

  public static Value createGeminiModelConfigForGoogleAiStudio(String apiKey) {
    return Constructor.createGeminiModelConfigForGoogleAiStudio(
        apiKey, GOOGLE_AI_STUDIO_GEMINI_PRO_ENDPOINT, GEMINI_PRO_TEXT_GENERATION_JSON_TEMPLATE);
  }

  public static Value createMediaPipeLlmInferenceModelConfig(String modelPath) {
    return Constructor.createMediaPipeLlmInferenceModelConfig(
        modelPath,
        /* maxTokens= */ 64,
        /* topK= */ 40,
        /* temperature= */ 0.8f,
        /* randomSeed= */ 100);
  }

  public static Value createOpenAiModelConfig(String apiKey) {
    return Constructor.createOpenAiModelConfig(
        apiKey, OPEN_AI_CHAT_COMPLETIONS_ENDPOINT, OPEN_AI_CHAT_COMPLETIONS_JSON_TEMPLATE);
  }

  @Nullable
  public static Value createOpenAiModelWithPromptChainDemo() {
    // Create prompt template to use.
    Value promptTemplate = Constructor.createPromptTemplate(PROMPT_TEMPLATE_FOR_DEMO);

    // Create Open AI chat completions model inference.
    Value openAiModel =
        Constructor.createModelInferenceWithConfig(
            OPEN_AI_MODEL_URI, createOpenAiModelConfig(OPEN_AI_API_KEY));

    // Create prompt, model inference chain.
    return Constructor.createSerialChain(
        new ArrayList<>(ImmutableList.of(promptTemplate, openAiModel)));
  }

  @Nullable
  public static Value createMediaPipeLlmInferenceModelWithPromptChainDemo() {
    // Create prompt template to use.
    Value promptTemplate = Constructor.createPromptTemplate(PROMPT_TEMPLATE_FOR_DEMO);

    // Create MediaPipe LLM Inference backed model inference.
    Value mediaPipeLlmInferenceModel =
        Constructor.createModelInferenceWithConfig(
            MEDIAPIPE_ON_DEVICE_MODEL_URI,
            createMediaPipeLlmInferenceModelConfig(MEDIAPIPE_GEMMA_GPU_ON_DEVICE_MODEL_PATH));

    // Create prompt, model inference chain.
    return Constructor.createSerialChain(
        new ArrayList<>(ImmutableList.of(promptTemplate, mediaPipeLlmInferenceModel)));
  }

  @Nullable
  public static Value createModelCascadeDemo() {
    // Create cloud inference computation. In this example, we use Gemini Pro through Google AI
    // Studio.
    Value cloudModel =
        Constructor.createModelInferenceWithConfig(
            GEMINI_MODEL_URI, createGeminiModelConfigForGoogleAiStudio(GOOGLE_AI_STUDIO_API_KEY));

    // Create on-device inference computation. In this example, we use MediaPipe's LlmInference
    // as backend for Gemma model.
    Value onDeviceModel =
        Constructor.createModelInferenceWithConfig(
            MEDIAPIPE_ON_DEVICE_MODEL_URI,
            createMediaPipeLlmInferenceModelConfig(MEDIAPIPE_GEMMA_GPU_ON_DEVICE_MODEL_PATH));

    // Create model cascade that first sends request to cloud model, and if it fails due to error
    // or network unavailability, routes the inference query to the on-device model.
    Value modelCascade =
        Constructor.createFallback(new ArrayList<>(ImmutableList.of(cloudModel, onDeviceModel)));

    // Create prompt template to use.
    Value promptTemplate = Constructor.createPromptTemplate(PROMPT_TEMPLATE_FOR_DEMO);

    // Create chain of prompt and model cascade.
    Value promptAndModelCascadeChain =
        Constructor.createSerialChain(
            new ArrayList<>(ImmutableList.of(promptTemplate, modelCascade)));
    return promptAndModelCascadeChain;
  }

  @Nullable
  public static Value readComputationFromFile() {
    try {
      InputStream stream = new FileInputStream(COMPUTATION_FILE_PATH);
      Value computation = Value.parseFrom(stream, getExtensionRegistry());
      stream.close();
      return computation;
    } catch (Exception e) {
      logger.atSevere().withCause(e).log(
          "Could not read Value proto from file: %s", e.getMessage());
    }
    return null;
  }

  @Nullable
  public static Value getComputation() {
    if (IS_READ_COMPUTATION_FROM_FILE) {
      return readComputationFromFile();
    } else {
      // Creating model cascade computation as this is the demo in Tutorial #1.
      // Please feel free to use other example computation workflows as covered in Tutorial #6, or
      // create your own computations!
      return createModelCascadeDemo();
    }
  }

  // Returns best available ExtensionRegistry.
  public static ExtensionRegistryLite getExtensionRegistry() {
    return ExtensionRegistryLite.getEmptyRegistry();
  }

  private Computations() {}
}
