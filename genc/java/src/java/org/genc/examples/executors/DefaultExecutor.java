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

package org.genc.examples.executors;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import src.java.org.genc.interop.backends.googleai.GoogleAiClient;
import src.java.org.genc.interop.backends.openai.OpenAiClient;
import src.java.org.genc.interop.backends.wolframalpha.WolframAlphaClient;
import src.java.org.genc.interop.network.JavaHttpClientImpl;

/** An executor used in GenC demos. */
public final class DefaultExecutor {
  static {
    System.loadLibrary("default_executor_jni");
  }

  public DefaultExecutor() {
    executorService = Executors.newFixedThreadPool(THREADPOOL_SIZE);
    httpClient = new JavaHttpClientImpl(executorService);
    googleAiClient = new GoogleAiClient(httpClient);
    openAiClient = new OpenAiClient(httpClient);
    wolframAlphaClient = new WolframAlphaClient(httpClient);
    executorHandle = createDefaultExecutor(openAiClient, googleAiClient, wolframAlphaClient);
  }

  public long getExecutorHandle() {
    return executorHandle;
  }

  public long cleanupExecutor() {
    executorService.shutdownNow();
    return cleanupExecutorState();
  }

  // Objects are referenced in C++ over JNI.
  public GoogleAiClient googleAiClient;
  public OpenAiClient openAiClient;
  public WolframAlphaClient wolframAlphaClient;

  private static final int THREADPOOL_SIZE = 4;
  private final ExecutorService executorService;
  private final JavaHttpClientImpl httpClient;
  private final long executorHandle;

  private native long createDefaultExecutor(
      OpenAiClient openAiClient,
      GoogleAiClient googleAiClient,
      WolframAlphaClient wolframAlphaClient);

  private native long cleanupExecutorState();
}
