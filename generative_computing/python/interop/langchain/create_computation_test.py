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
from generative_computing.proto.v0 import computation_pb2 as pb
from generative_computing.python.interop.langchain import create_computation
from generative_computing.python.interop.langchain import custom_model


class CreateComputationTest(absltest.TestCase):

  def test_model(self):
    llm = custom_model.CustomModel(uri="some_model")
    for comp in [
        create_computation.create_computation_from_llm(llm),
        create_computation.create_computation(llm),
    ]:
      self.assertIsInstance(comp, pb.Computation)
      self.assertEqual(comp.WhichOneof("computation"), "model")
      self.assertEqual(comp.model.model_id.uri, "some_model")

  def test_prompt_template(self):
    prompt_template = prompts.PromptTemplate(
        input_variables=["location"],
        template="Q: What should I pack for a trip to {location}? A: ",
    )
    for comp in [
        create_computation.create_computation_from_prompt_template(
            prompt_template
        ),
        create_computation.create_computation(prompt_template),
    ]:
      self.assertIsInstance(comp, pb.Computation)
      self.assertEqual(comp.WhichOneof("computation"), "prompt_template")
      self.assertEqual(
          comp.prompt_template.template_string, prompt_template.template
      )

  def test_chain(self):
    my_chain = chains.LLMChain(
        llm=custom_model.CustomModel(uri="some_model"),
        prompt=prompts.PromptTemplate(
            input_variables=["location"],
            template="Q: What should I pack for a trip to {location}? A: ",
        ),
    )
    create_computation.create_computation(my_chain)
    # TODO(b/295042499): Add test code once chains are supported.


if __name__ == "__main__":
  absltest.main()
