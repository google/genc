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
 * An example showing a simple chain using Gemini-Pro model inference calls on Vertex AI as backend.
 *
 * <p>For this demo, we need: 1) Access token to authenticate Gemini-Pro model inference access on
 * Vertex AI. Follow instructions to get the access token (run: gcloud auth print-access-token):
 * https://cloud.google.com/vertex-ai/docs/authentication#rest
 *
 * <p>2) Gemini-Pro endpoint URL for Vertex AI. Example endpoint:
 * "https://us-central1-aiplatform.googleapis.com/v1/projects/<projectId>/locations/us-central1/
 * publishers/google/models/gemini-1.0-pro:streamGenerateContent";
 *
 * <p>where projectId is the Id of your Google Cloud project. You can either use this endpoint with
 * your Project Id used in Google Cloud platform. If you don't have a project Id, follow
 * instructions to set up a project and get the project id:
 * https://cloud.google.com/vertex-ai/docs/start/cloud-environment
 *
 * <p>For more details on endpoint and other choices on regions, see:
 * https://cloud.google.com/vertex-ai/generative-ai/docs/model-reference/gemini
 *
 * <p>USAGE: bazel run genc/java/src/java/org/genc/examples:gemini_on_vertex_ai -- <access_token>
 * <endpoint> <prompt>
 */
final class GeminiOnVertexAiDemo {
  // Default json request template for Gemini-Pro calls on Vertex AI.Please feel free to edit this
  // per your use-case. See other configuration options:
  // https://cloud.google.com/vertex-ai/generative-ai/docs/model-reference/gemini
  public static final String VERTEX_AI_GEMINI_PRO_TEXT_GENERATION_JSON_TEMPLATE =
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
  public static final String PROMPT_TEMPLATE_FOR_DEMO = "Give me a recipe for {dish}?";

  public static final String GEMINI_MODEL_URI = "/cloud/gemini";

  public static Value createGeminiModelConfigForVertexAi(String accessToken, String endpoint) {
    return Constructor.createGeminiModelConfigForVertexAi(
        accessToken, endpoint, VERTEX_AI_GEMINI_PRO_TEXT_GENERATION_JSON_TEMPLATE);
  }

  @Nullable
  public static Value createGeminiModelWithPromptChain(String accessToken, String endpoint) {
    // Create prompt template to use.
    Value promptTemplate = Constructor.createPromptTemplate(PROMPT_TEMPLATE_FOR_DEMO);

    // Create Gemini-Pro model inference with Vertex AI as cloud backend.
    Value vertexAiGeminiModel =
        Constructor.createModelInferenceWithConfig(
            GEMINI_MODEL_URI, createGeminiModelConfigForVertexAi(accessToken, endpoint));

    // Create prompt, model inference chain.
    return Constructor.createSerialChain(
        new ArrayList<>(ImmutableList.of(promptTemplate, vertexAiGeminiModel)));
  }

  public static void main(String[] args) {
    DefaultExecutor executor = new DefaultExecutor();
    try {
      if (args.length != 3) {
        System.out.println(
            "Usage: bazel run genc/java/src/java/org/genc/examples:gemini_on_vertex_ai"
                + " -- <access_token> <endpoint> <prompt>");
        return;
      }

      Value ir = createGeminiModelWithPromptChain(args[0], args[1]);
      Runner runner = Runner.create(ir, executor.getExecutorHandle());
      String result = runner.call(args[2]);
      System.out.println(result);
    } finally {
      executor.cleanupExecutor();
    }
  }

  private GeminiOnVertexAiDemo() {}
}
