# Copyright 2024, The GenC Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Test for model_inference.py."""

from absl.testing import absltest

from genc.python import authoring
from genc.python.interop.llamacpp import model_inference


class ModelInferenceTest(absltest.TestCase):

  def test_model_inference(self):
    @authoring.traced_computation
    def foo(x):
      return model_inference.model_inference(
          "/device/llamacpp",
          "/gemma-7b-it.gguf",
          num_threads=100,
          max_tokens=200)(x)

    @authoring.traced_computation
    def bar(x):
      return authoring.model_inference_with_config[{
          "model_uri": "/device/llamacpp",
          "model_config": {
              "model_path": "/gemma-7b-it.gguf",
              "num_threads": 100,
              "max_tokens": 200}}](x)

    self.assertEqual(str(foo.portable_ir), str(bar.portable_ir))


if __name__ == "__main__":
  absltest.main()
