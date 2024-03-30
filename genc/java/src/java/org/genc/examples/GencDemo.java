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

import org.genc.Runner;
import org.genc.Value;
import org.genc.authoring.Constructor;
import org.genc.examples.executors.DefaultExecutor;

/** Demo client for testing deployment in Java. */
public final class GencDemo {
  public static void main(String[] args) {
    Value ir = Constructor.readComputationFromFile(args[0]);
    DefaultExecutor executor = new DefaultExecutor();
    Runner runner = Runner.create(ir, executor.getExecutorHandle());
    String result = runner.call(args[1]);
    System.out.println(result);
  }

  private GencDemo() {}
}