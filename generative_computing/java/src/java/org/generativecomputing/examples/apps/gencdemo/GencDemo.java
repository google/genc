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
import org.generativecomputing.Runner;
import org.generativecomputing.Value;

/** Main activity for the GencDemo app. */
public class GencDemo extends Activity {
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    try {
      InputStream stream = new FileInputStream("/data/local/tmp/google_ai_gemini.pb");
      Value computation = Value.parseFrom(stream, getExtensionRegistry());
      executor = new DefaultAndroidExecutor(getApplicationContext());
      runner = Runner.create(computation, executor.getExecutorHandle());
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
            String text = query.getText().toString();
            String responseString = "";
            try {
              responseString = runner.call(text);
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
    executor.cleanupAndroidExecutor();
    super.onDestroy();
  }

  // Returns best available ExtensionRegistry.
  public static ExtensionRegistryLite getExtensionRegistry() {
    return ExtensionRegistryLite.getEmptyRegistry();
  }

  private DefaultAndroidExecutor executor;
  private Runner runner;
}
