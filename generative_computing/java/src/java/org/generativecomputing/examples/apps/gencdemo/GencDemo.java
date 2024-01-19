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

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import com.google.protobuf.ExtensionRegistryLite;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import org.chromium.net.CronetEngine;
import org.generativecomputing.Runner;
import org.generativecomputing.Value;
import src.java.org.generativecomputing.interop.backends.openai.OpenAiClient;
import src.java.org.generativecomputing.interop.network.CronetEngineProvider;
import src.java.org.generativecomputing.interop.network.HttpClientImpl;

/** Main activity for the GencDemo app. */
public class GencDemo extends Activity {
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    cronetEngine = CronetEngineProvider.createCronetEngine(getApplicationContext());
    cronetCallbackExecutorService = Executors.newFixedThreadPool(4);
    httpClient = new HttpClientImpl(cronetEngine, cronetCallbackExecutorService);
    openAiClient = new OpenAiClient(httpClient, OPENAI_SERVER_URL, OPEN_AI_API_KEY);
    try {
      InputStream stream = new FileInputStream("/data/local/tmp/openai_chatgpt.pb");
      Value computation = Value.parseFrom(stream, getExtensionRegistry());
      runner = Runner.create(computation, createAndroidExecutor(openAiClient));
    } catch (Exception e) {
      Log.e("GencDemo", e.toString());
    }

    // User input
    EditText query = findViewById(R.id.query);

    // Response
    TextView response = findViewById(R.id.response);
    TextView verbose = findViewById(R.id.verbose);

    // Button calls inference on device when pressed
    Button onDeviceButton = findViewById(R.id.od);
    onDeviceButton.setOnClickListener(
        new View.OnClickListener() {
          @Override
          public void onClick(View view) {
            String newText = query.getText().toString();
            Value val = Value.newBuilder().setStr(newText).build();
            Value[] args = {val};
            String responseString = "";
            try {
              responseString = runner.call(args);
            } catch (RuntimeException e) {
              Log.e("GencDemo", e.toString());
            }
            response.setText(responseString);
            String verboseString;
            if (responseString.isEmpty()) {
              verboseString = "Failed to get response. See error logs for more details.";
            } else {
              verboseString = "Success!";
            }
            verbose.setText(verboseString);
          }
        });
  }

  @Override
  public void onDestroy() {
    cleanupAndroidExecutorState();
    super.onDestroy();
  }

  public CronetEngine getCronetEngine() {
    return cronetEngine;
  }

  public OpenAiClient openAiClient;

  // Returns best available ExtensionRegistry.
  public static ExtensionRegistryLite getExtensionRegistry() {
    return ExtensionRegistryLite.getEmptyRegistry();
  }

  private static final String OPENAI_SERVER_URL = "https://api.openai.com/v1/chat/completions";
  // Fill in the OpenAI API key.
  private static final String OPEN_AI_API_KEY = "";

  private CronetEngine cronetEngine;
  private ExecutorService cronetCallbackExecutorService;
  private HttpClientImpl httpClient;
  private Runner runner;

  private native long createAndroidExecutor(OpenAiClient openAiClient);

  private native long cleanupAndroidExecutorState();

  static {
    System.loadLibrary("app");
  }
}
