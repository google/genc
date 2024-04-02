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
 * An example showing OpenAI Chat completions as backend in a prompt template, model inference
 * chain.
 *
 * <p>See instructions to get an API key to use for Open AI chat completions:
 * https://platform.openai.com/api-keys
 *
 * <p>USAGE: bazel run genc/java/src/java/org/genc/examples:open_ai_demo -- <api_key> <prompt>
 */
public final class OpenAiDemo {
  // Open AI server url for chat completions API.
  private static final String OPEN_AI_CHAT_COMPLETIONS_ENDPOINT =
      "https://api.openai.com/v1/chat/completions";

  // Default prompt template used in demo. Please feel free to edit this to use a different prompt.
  private static final String PROMPT_TEMPLATE_FOR_DEMO = "Write a haiku about {topic}?";

  // Example json request template for Open AI calls. Please feel free to edit this per your
  // use-case. See configuration at https://platform.openai.com/docs/guides/text-generation.
  private static final String OPEN_AI_CHAT_COMPLETIONS_JSON_TEMPLATE =
      "{\n"
          + "  \"model\":\"gpt-3.5-turbo\",\n"
          + "  \"messages\":[{\"role\":\"user\",\"content\":\"PROMPT REPLACES THIS\"}]\n"
          + "}";

  private static final String OPEN_AI_MODEL_URI = "/openai/chatgpt";

  private static Value createOpenAiModelConfig(String apiKey) {
    return Constructor.createOpenAiModelConfig(
        apiKey, OPEN_AI_CHAT_COMPLETIONS_ENDPOINT, OPEN_AI_CHAT_COMPLETIONS_JSON_TEMPLATE);
  }

  @Nullable
  private static Value createOpenAiModelWithPromptChain(String apiKey) {
    // Create prompt template to use.
    Value promptTemplate = Constructor.createPromptTemplate(PROMPT_TEMPLATE_FOR_DEMO);

    // Create Open AI chat completions model inference.
    Value openAiModel =
        Constructor.createModelInferenceWithConfig(
            OPEN_AI_MODEL_URI, createOpenAiModelConfig(apiKey));

    // Create prompt, model inference chain.
    return Constructor.createSerialChain(
        new ArrayList<>(ImmutableList.of(promptTemplate, openAiModel)));
  }

  public static void main(String[] args) {
    if (args.length != 2) {
      System.out.println(
          "Usage: bazel run genc/java/src/java/org/genc/examples:open_ai_demo -- <api_key>"
              + " <prompt>");
      return;
    }

    DefaultExecutor executor = new DefaultExecutor();
    Value ir = createOpenAiModelWithPromptChain(args[0]);
    Runner runner = Runner.create(ir, executor.getExecutorHandle());
    String result = runner.call(args[1]);
    System.out.println(result);
  }

  private OpenAiDemo() {}
}
