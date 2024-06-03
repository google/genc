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
"""Test for create_config.py."""

from absl.testing import absltest

from genc.python.interop.llamacpp import create_config


class CreateConfigTest(absltest.TestCase):

  def test_create_config(self):
    self.assertEqual(
        create_config.create_config(
            "/gemma-7b-it.gguf", num_threads=100, max_tokens=200),
        {
            "model_path": "/gemma-7b-it.gguf",
            "num_threads": 100,
            "max_tokens": 200
        })


if __name__ == "__main__":
  absltest.main()
