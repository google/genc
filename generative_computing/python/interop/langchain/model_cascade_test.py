# Copyright 2023, The Generative Computing Authors.
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
"""Test for model_cascade.py."""

from absl.testing import absltest
import langchain
from generative_computing.python.interop.langchain import custom_model
from generative_computing.python.interop.langchain import model_cascade


class ModelCascadeTest(absltest.TestCase):

  def test_creation(self):
    llm1 = custom_model.CustomModel(uri='some_model')
    llm2 = custom_model.CustomModel(uri='some_other_model')
    llm3 = model_cascade.ModelCascade(models=[llm1, llm2])
    self.assertIsInstance(llm3, langchain.llms.base.LLM)
    self.assertLen(llm3.models, 2)
    self.assertEqual(
        llm3._llm_type,
        'ModelCascade(CustomModel(some_model), CustomModel(some_other_model))',
    )


if __name__ == '__main__':
  absltest.main()
