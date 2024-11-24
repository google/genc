# Copyright 2023, The GenC Authors.
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
"""A simple example that demonstrates native authoring and runtime APIs."""

from absl.testing import absltest
import genc as genc
from genc.python.examples import executor

# pylint:disable=missing-class-docstring
# pylint:disable=missing-function-docstring]


class AuthoringTest(absltest.TestCase):

  def test_something(self):
    my_executor = executor.create_default_executor()
    genc.runtime.set_default_executor(my_executor)

    @genc.authoring.traced_computation
    def comp(x):
      template_str = "Q: What should I pack for a trip to {location}? A: "
      prompt = genc.authoring.prompt_template[template_str](x)
      model_output = genc.authoring.model_inference["test_model"](prompt)
      return model_output

    result = comp("a grocery store")
    expected_result = (
        'This is an output from a test model in response to "Q: What should I'
        ' pack for a trip to a grocery store? A: ".'
    )
    self.assertEqual(result, expected_result)


if __name__ == "__main__":
  absltest.main()
