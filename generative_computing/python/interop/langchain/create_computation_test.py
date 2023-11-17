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
from generative_computing.python import authoring
from generative_computing.python.interop.langchain import create_computation
from generative_computing.python.interop.langchain import custom_chain
from generative_computing.python.interop.langchain import custom_model
from generative_computing.python.interop.langchain import model_cascade


class CreateComputationTest(absltest.TestCase):

  def test_model(self):
    llm = custom_model.CustomModel(uri="some_model")
    for comp in [
        create_computation.create_computation_from_llm(llm),
        create_computation.create_computation(llm),
    ]:
      expected_comp = authoring.create_model("some_model")
      self.assertEqual(str(comp), str(expected_comp))

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
      expected_comp = authoring.create_prompt_template(prompt_template.template)
      self.assertEqual(str(comp), str(expected_comp))

  def test_chain(self):
    my_chain = chains.LLMChain(
        llm=custom_model.CustomModel(uri="some_model"),
        prompt=prompts.PromptTemplate(
            input_variables=["location"],
            template="Q: What should I pack for a trip to {location}? A: ",
        ),
    )
    comp = create_computation.create_computation(my_chain)
    expected_comp = authoring.create_basic_chain([
        authoring.create_prompt_template(
            "Q: What should I pack for a trip to {location}? A: ",
        ),
        authoring.create_model("some_model"),
    ])
    self.assertEqual(str(comp), str(expected_comp))

  def test_nested_hybrid_custom_chain(self):
    lc_llm_chain = chains.LLMChain(
        llm=custom_model.CustomModel(uri="model1"),
        prompt=prompts.PromptTemplate(
            input_variables=["location"],
            template="Q: What should I pack for a trip to {location}? A: ",
        ),
    )
    genc_llm2 = authoring.create_model("model2")
    my_chain = custom_chain.CustomChain() | lc_llm_chain | genc_llm2 | 4
    langchain_computation = create_computation.create_computation(my_chain)

    expected_sub_chain = authoring.create_basic_chain([
        authoring.create_prompt_template(
            "Q: What should I pack for a trip to {location}? A: ",
        ),
        authoring.create_model("model1"),
    ])
    expected_chain = authoring.create_loop_chain_combo(
        4, [expected_sub_chain, genc_llm2]
    )
    self.assertEqual(langchain_computation, expected_chain)

  def test_cascade(self):
    llm1 = custom_model.CustomModel(uri="some_model")
    llm2 = custom_model.CustomModel(uri="some_other_model")
    llm3 = model_cascade.ModelCascade(models=[llm1, llm2])
    comp3 = create_computation.create_computation(llm3)
    comp4 = authoring.create_fallback([
        authoring.create_model("some_model"),
        authoring.create_model("some_other_model"),
    ])
    self.assertEqual(str(comp3), str(comp4))


if __name__ == "__main__":
  absltest.main()
