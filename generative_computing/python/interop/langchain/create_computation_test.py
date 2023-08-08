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
"""Test for create_computation.py."""

from absl.testing import absltest
from langchain import chains
from langchain import prompts
from generative_computing.python.interop.langchain import create_computation
from generative_computing.python.interop.langchain import custom_model


class CreateComputationTest(absltest.TestCase):

  def test_simple(self):
    my_chain = chains.LLMChain(
        llm=custom_model.CustomModel(uri="some_model"),
        prompt=prompts.PromptTemplate(
            input_variables=["location"],
            template="Q: What should I pack for a trip to {location}? A: ",
        ),
    )

    create_computation.create_computation(my_chain)


if __name__ == "__main__":
  absltest.main()
