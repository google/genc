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
 * An example showing Wolfram Alpha as math tool backend, within a chain.
 *
 * <p>See instructions to get an API key from Wolfram Alpha: https://www.wolframalpha.com/api
 *
 * <p>USAGE: bazel run genc/java/src/java/org/genc/examples:wolfram_alpha_demo -- <api_key> <prompt>
 */
final class WolframAlphaDemo {
  public static final String PROMPT_TEMPLATE_FOR_DEMO = "What is the result of {mathexpression}?";

  @Nullable
  public static Value createWolframAlphaMathTool(String apiKey) {
    // Create prompt template to use.
    Value promptTemplate = Constructor.createPromptTemplate(PROMPT_TEMPLATE_FOR_DEMO);

    // Create Math tool that uses Wolfram Alpha to answer math questions.
    Value wolframAlphaClient = Constructor.createWolframAlpha(apiKey);

    // Create prompt, tool chain.
    return Constructor.createSerialChain(
        new ArrayList<>(ImmutableList.of(promptTemplate, wolframAlphaClient)));
  }

  public static void main(String[] args) {
    DefaultExecutor executor = new DefaultExecutor();
    try {
      if (args.length != 2) {
        System.out.println(
            "Usage: bazel run genc/java/src/java/org/genc/examples:wolfram_alpha_demo -- <api_key>"
                + " <prompt>");
        return;
      }

      Value ir = createWolframAlphaMathTool(args[0]);
      Runner runner = Runner.create(ir, executor.getExecutorHandle());
      String result = runner.call(args[1]);
      System.out.println(result);
    } finally {
      executor.cleanupExecutor();
    }
  }

  private WolframAlphaDemo() {}
}
