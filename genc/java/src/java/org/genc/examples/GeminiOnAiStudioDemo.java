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
package org.genc.examples;

import com.google.common.collect.ImmutableList;
import java.util.ArrayList;
import javax.annotation.Nullable;
import org.genc.Value;
import org.genc.authoring.Constructor;
import org.genc.examples.executors.DefaultExecutor;
import org.genc.runtime.Runner;

/**
 * An example showing a simple chain using Gemini-Pro model inference calls on Google AI Studio as
 * backend.
 *
 * <p>For this demo, we need an API key to use for Gemini-Pro model inference through Google AI
 * Studio. Follow instructions to get API key: https://ai.google.dev/tutorials/rest_quickstart
 *
 * <p>USAGE: bazel run genc/java/src/java/org/genc/examples:gemini_on_ai_studio -- <api_key>
 * <prompt>
 */
final class GeminiOnAiStudioDemo {
  // Gemini-Pro endpoint URL from Google AI Studio.
  public static final String GOOGLE_AI_STUDIO_GEMINI_PRO_ENDPOINT =
      "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent";

  // Default json request template for Gemini-Pro calls on Google Studio. Please feel free to edit
  // this per your use-case. See more configuration settings and options:
  // https://ai.google.dev/tutorials/rest_quickstart#configuration.
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

  // Default prompt template used in demos. Please feel free to edit this per your liking.
  public static final String PROMPT_TEMPLATE_FOR_DEMO = "Write a poem about {topic}?";

  public static final String GEMINI_MODEL_URI = "/cloud/gemini";

  public static Value createGeminiModelConfigForGoogleAiStudio(String apiKey) {
    return Constructor.createGeminiModelConfigForGoogleAiStudio(
        apiKey, GOOGLE_AI_STUDIO_GEMINI_PRO_ENDPOINT, GEMINI_PRO_TEXT_GENERATION_JSON_TEMPLATE);
  }

  @Nullable
  public static Value createGeminiModelWithPromptChain(String apiKey) {
    // Create prompt template to use.
    Value promptTemplate = Constructor.createPromptTemplate(PROMPT_TEMPLATE_FOR_DEMO);

    // Create Gemini-Pro model inference with Google AI Studio as cloud backend.
    Value geminiModel =
        Constructor.createModelInferenceWithConfig(
            GEMINI_MODEL_URI, createGeminiModelConfigForGoogleAiStudio(apiKey));

    // Create prompt, model inference chain.
    return Constructor.createSerialChain(
        new ArrayList<>(ImmutableList.of(promptTemplate, geminiModel)));
  }

  public static void main(String[] args) {
    if (args.length != 2) {
      System.out.println(
          "Usage: bazel run genc/java/src/java/org/genc/examples:gemini_on_ai_studio"
              + " -- <api_key> <prompt>");
      return;
    }

    DefaultExecutor executor = new DefaultExecutor();
    Value ir = createGeminiModelWithPromptChain(args[0]);
    Runner runner = Runner.create(ir, executor.getExecutorHandle());
    String result = runner.call(args[1]);
    System.out.println(result);
  }

  private GeminiOnAiStudioDemo() {}
}
