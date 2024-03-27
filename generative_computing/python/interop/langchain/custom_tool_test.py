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
"""Test for custom_tool.py."""

from absl.testing import absltest
from langchain import tools
from generative_computing.python.interop.langchain import custom_chain
from generative_computing.python.interop.langchain import custom_tool


class CustomToolTest(absltest.TestCase):

  def test_creation(self):
    test_computation = custom_chain.CustomChain(chained_ops=[])
    tool = custom_tool.CustomTool(
        name='test_tool',
        description='test tool description',
        computation=test_computation,
    )
    self.assertIsInstance(tool, tools.BaseTool)
    self.assertEqual(tool.name, 'test_tool')
    self.assertEqual(tool.description, 'test tool description')
    self.assertEqual(tool.computation, test_computation)


if __name__ == '__main__':
  absltest.main()
