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
 * An example showing a simple chain with on-device model inference calls with Llamacpp as backend.
 *
 * <p>For this demo, we need an on-device model, compatible with Llamacpp. As an example, you can
 * fetch gemma-2b-it-q4_k_m.gguf from https://huggingface.co/lmstudio-ai/gemma-2b-it-GGUF/tree/main,
 * and place it somewhere on your local filesystem within the docker container, where you'll be
 * running this demo. Please take a note of the local path where you placed this model, as you will
 * be supplying it when running the demo.
 *
 * <p>USAGE: bazel run genc/java/src/java/org/genc/examples:local_model_with_llama_cpp --
 * <model_path> <prompt>
 */
final class LocalModelWithLlamacpp {
  public static final int LLAMACPP_ON_DEVICE_MODEL_NUM_THREADS = 4;
  public static final int LLAMACPP_ON_DEVICE_MODEL_MAX_TOKENS = 64;
  public static final String LLAMACPP_MODEL_URI = "/device/gemma";

  // Default prompt template used in demo. Please feel free to edit this per your liking.
  public static final String PROMPT_TEMPLATE_FOR_DEMO = "Tell me about {topic}";

  public static Value createLlamacppModelConfig(String modelPath) {
    return Constructor.createLlamaModelConfig(
        modelPath, LLAMACPP_ON_DEVICE_MODEL_NUM_THREADS, LLAMACPP_ON_DEVICE_MODEL_MAX_TOKENS);
  }

  @Nullable
  public static Value createLocalModelPromptChain(String modelPath) {
    // Create prompt template to use.
    Value promptTemplate = Constructor.createPromptTemplate(PROMPT_TEMPLATE_FOR_DEMO);

    // Create on-device inference computation.
    Value onDeviceModel =
        Constructor.createModelInferenceWithConfig(
            LLAMACPP_MODEL_URI, createLlamacppModelConfig(modelPath));

    // Create prompt, model inference chain.
    return Constructor.createSerialChain(
        new ArrayList<>(ImmutableList.of(promptTemplate, onDeviceModel)));
  }

  public static void main(String[] args) {
    DefaultExecutor executor = new DefaultExecutor();
    try {
      if (args.length != 2) {
        System.out.println(
            "Usage: bazel run genc/java/src/java/org/genc/examples:local_model_with_llama_cpp --"
                + " <model_path> <prompt>");
        return;
      }

      Value ir = createLocalModelPromptChain(args[0]);
      Runner runner = Runner.create(ir, executor.getExecutorHandle());
      String result = runner.call(args[1]);
      System.out.println(result);
    } finally {
      executor.cleanupExecutor();
    }
  }

  private LocalModelWithLlamacpp() {}
}
