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
"""Utility function for creating `Computation` protos from LangChain objects."""

import langchain
from langchain import agents

from genc.python import authoring
from genc.python.base import to_from_value_proto
from genc.python.interop.langchain import custom_chain
from genc.python.interop.langchain import custom_model
from genc.python.interop.langchain import model_cascade


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
    if llm.config:
      return authoring.create_model_with_config(
          llm.uri, to_from_value_proto.to_value_proto(llm.config)
      )
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
    comp = authoring.create_serial_chain(
        [create_computation_from_prompt_template(chain.prompt), comp]
    )
  return comp


def create_computation_from_agent(agent):
  """Creates a `Computation` proto instance from a LangChain agent.

  Args:
    agent: An instance of `Agent`.

  Returns:
    A corresponding instance of `pb.Value`.
  """
  model_call = create_computation_from_llm(agent.llm_chain.llm)

  print_result = authoring.constructors.create_logger()
  stop_when_finish = authoring.constructors.create_regex_partial_match("Finish")
  parse_thought_action = authoring.constructors.create_custom_function(
      "/react/parse_thought_action"
  )
  format_observation = authoring.constructors.create_custom_function(
      "/react/format_observation"
  )

  read_context = authoring.constructors.create_custom_function(
      "/local_cache/read"
  )
  add_to_context = authoring.constructors.create_custom_function(
      "/local_cache/write"
  )
  evict_context = authoring.constructors.create_custom_function(
      "/local_cache/remove"
  )

  tool_computation = create_computation_from_custom_chain(
      agent.tools_list[0].computation
  )
  react_loop = authoring.constructors.create_repeated_conditional_chain(
      agent.max_iterations,
      [
          read_context,
          model_call,
          parse_thought_action,
          print_result,
          add_to_context,
          stop_when_finish,
          tool_computation,
          format_observation,
          print_result,
          add_to_context,
      ],
  )

  instruction_template = authoring.constructors.create_prompt_template(
      agent.llm_chain.prompt.template
  )

  reagent_steps = [
      # Remove previous context before processing new round of request.
      evict_context,
      instruction_template,
      add_to_context,
      print_result,
      react_loop,
  ]
  if agent.return_intermediate_steps:
    reagent_steps.append(read_context)
  return authoring.constructors.create_serial_chain(reagent_steps)


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

  if chain.num_iteration > 1:
    return authoring.create_repeated_conditional_chain(
        chain.num_iteration, computation_list
    )
  return authoring.create_serial_chain(computation_list)


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
  if isinstance(langchain_object, agents.agent.Agent):
    return create_computation_from_agent(langchain_object)
  if isinstance(langchain_object, langchain.chains.base.Chain):
    return create_computation_from_custom_chain(langchain_object)
  raise TypeError(
      "Unrecognized type of LangChain object {}.".format(
          str(type(langchain_object))
      )
  )
