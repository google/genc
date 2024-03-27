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
"""Test for custom_model.py."""

from absl.testing import absltest
from langchain.llms.base import LLM  # pylint:disable=g-importing-member
from generative_computing.python.interop.langchain import custom_model


class CustomModelTest(absltest.TestCase):

  def test_creation(self):
    llm = custom_model.CustomModel(uri='some_model')
    self.assertIsInstance(llm, LLM)
    self.assertEqual(llm.uri, 'some_model')
    self.assertEqual(llm._llm_type, 'CustomModel(some_model)')

  def test_creation_with_config(self):
    test_config = {'test_key_1': 'test_value_1', 'test_key_2': 100}
    llm = custom_model.CustomModel(uri='some_model', config=test_config)
    self.assertIsInstance(llm, LLM)
    self.assertEqual(llm.uri, 'some_model')
    self.assertEqual(llm._llm_type, 'CustomModel(some_model)')
    self.assertEqual(llm.config, test_config)


if __name__ == '__main__':
  absltest.main()
