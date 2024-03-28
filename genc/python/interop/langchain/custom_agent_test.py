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
"""Test for custom_agent.py."""

from absl.testing import absltest
from langchain import agents
from langchain import chains
from langchain import prompts
from genc.python.interop.langchain import custom_agent
from genc.python.interop.langchain import custom_chain
from genc.python.interop.langchain import custom_model
from genc.python.interop.langchain import custom_tool


class CustomAgentTest(absltest.TestCase):

  def test_creation(self):
    test_chain = chains.LLMChain(
        llm=custom_model.CustomModel(uri='test_model'),
        prompt=prompts.PromptTemplate(
            input_variables=['topic'],
            template='Q: Tell me about {topic}? A: ',
        ),
    )
    test_computation = custom_chain.CustomChain(chained_ops=[])
    tool = custom_tool.CustomTool(
        name='test_tool',
        description='test tool description',
        computation=test_computation,
    )

    agent = custom_agent.CustomAgent(
        llm_chain=test_chain, tools_list=[tool], max_iterations=5
    )

    self.assertIsInstance(agent, agents.Agent)
    self.assertEqual(agent.llm_chain, test_chain)
    self.assertEqual(agent.tools_list[0], tool)


if __name__ == '__main__':
  absltest.main()
