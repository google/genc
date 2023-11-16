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
"""Utility function for creating `Computation` protos from LangChain objects."""

import langchain
from generative_computing.python import authoring
from generative_computing.python.interop.langchain import custom_chain
from generative_computing.python.interop.langchain import custom_model
from generative_computing.python.interop.langchain import model_cascade


def create_computation_from_llm(llm):
  """Creates a `Computation` proto instance from a LangChain model.

  Args:
    llm: An instance of `CustomModel` (the only model type currently supported).

  Returns:
    An instance of `pb.Value` with an embedded `Model`.

  Raises:
    TypeError: on an unrecognized type of model.
    ValueError: if the model URI or other key parameters are not defined.
  """
  if isinstance(llm, custom_model.CustomModel):
    if not llm.uri:
      raise ValueError("Model URI is required.")
    return authoring.create_model(llm.uri)
  elif isinstance(llm, model_cascade.ModelCascade):
    return authoring.create_fallback(
        [create_computation(x) for x in llm.models]
    )
  else:
    # TODO(b/295042550): Add support other types of models native to LangChain.
    raise TypeError("Unrecognized type of model.")


def create_computation_from_prompt_template(prompt_template):
  """Creates a `Computation` proto instance from a LangChain prompt template.

  Args:
    prompt_template: An instance of `PromptTemplate`.

  Returns:
    An instance of `pb.Value` with an embedded `PromptTemplate`.

  Raises:
    TypeError: on an unrecognized type of argument.
  """
  if not isinstance(prompt_template, langchain.PromptTemplate):
    raise TypeError(
        "Unrecognized type of argument {}.".format(type(prompt_template))
    )

  return authoring.create_prompt_template(prompt_template.template)


def create_computation_from_chain(chain):
  """Creates a `Computation` proto instance from a LangChain chain.

  Args:
    chain: An instance of LLMChain object.

  Returns:
    A corresponding instance of `pb.Value`.
  """
  comp = create_computation_from_llm(chain.llm)
  if chain.prompt:
    comp = authoring.create_basic_chain(
        [create_computation_from_prompt_template(chain.prompt), comp]
    )
  return comp


def create_computation_from_custom_chain(chain):
  """Creates a `Computation` proto instance from a LangChain CustomChain.

  Args:
    chain: An instance of CustomChain object.

  Returns:
    A corresponding instance of `pb.Value`.
  """

  computation_list = []
  for op in chain.chained_ops:
    comp = op
    if isinstance(op, custom_chain.CustomChain) or isinstance(
        op, langchain.chains.llm.LLMChain
    ):
      comp = create_computation(comp)
    computation_list.append(comp)

  return authoring.create_basic_chain(computation_list)


def create_computation(langchain_object):
  """Creates a `Computation` proto instance from a LangChain object.

  Args:
    langchain_object: An object (e.g., a chain) created using LangChain APIs.

  Returns:
    An instance of the `Computation` proto.

  Raises:
    TypeError: on an unrecognied type of object.
  """
  if isinstance(langchain_object, langchain.llms.base.LLM):
    return create_computation_from_llm(langchain_object)
  if isinstance(langchain_object, langchain.PromptTemplate):
    return create_computation_from_prompt_template(langchain_object)
  if isinstance(langchain_object, langchain.chains.llm.LLMChain):
    return create_computation_from_chain(langchain_object)
  if isinstance(langchain_object, langchain.chains.base.Chain):
    return create_computation_from_custom_chain(langchain_object)
  raise TypeError(
      "Unrecognized type of LangChain object {}.".format(
          str(type(langchain_object))
      )
  )
