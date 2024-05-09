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

package org.genc.examples.apps.gencdemo;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import com.google.common.flogger.FluentLogger;
import org.genc.Value;
import org.genc.runtime.Runner;

/** Main activity for the GencDemo app. */
public class GencDemo extends Activity {
  private static final FluentLogger logger = FluentLogger.forEnclosingClass();
  private DefaultAndroidExecutor executor;
  private Runner runner;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    try {
      Value computation = Computations.getComputation();
      executor = new DefaultAndroidExecutor(getApplicationContext());
      runner = Runner.create(computation, executor.getExecutorHandle());
    } catch (RuntimeException e) {
      logger.atSevere().withCause(e).log(
          "Error occured in creating the computation: %s", e.getMessage());
    }

    // User input
    EditText query = findViewById(R.id.query);

    // Response
    TextView response = findViewById(R.id.response);
    TextView verbose = findViewById(R.id.verbose);

    // Inference Handler
    Handler infHandler = new Handler();

    final String generatingResponse = "Generating Response...";

    // Button calls inference on device when pressed
    Button onDeviceButton = findViewById(R.id.od);
    onDeviceButton.setOnClickListener(
        new View.OnClickListener() {
          @Override
          public void onClick(View view) {
            final String text = query.getText().toString();
            response.setText("");
            verbose.setText(generatingResponse);
            // Runs inference on a new thread.
            infHandler.post(
                () -> {
                  String responseString = "";
                  try {
                    responseString = runner.call(text);
                  } catch (RuntimeException e) {
                    logger.atSevere().withCause(e).log(
                        "Error occured in running the computation: %s", e.getMessage());
                  }
                  response.setText(responseString);
                  String verboseString;
                  if (responseString.isEmpty()) {
                    verboseString = "Failed to get response. See error logs for more details.";
                  } else {
                    verboseString = "Success!";
                  }
                  verbose.setText(verboseString);
                });
          }
        });
  }

  @Override
  public void onDestroy() {
    executor.cleanupAndroidExecutor();
    super.onDestroy();
  }
}
