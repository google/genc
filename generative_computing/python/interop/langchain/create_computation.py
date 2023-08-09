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
from generative_computing.proto.v0 import computation_pb2 as pb
from generative_computing.python.interop.langchain import custom_model


def create_computation_from_llm(llm):
  """Creates a `Computation` proto instance from a LangChain model.

  Args:
    llm: An instance of `CustomModel` (the only model type currently supported).

  Returns:
    An instance of `pb.Computation` with an embedded `ModelCall`.

  Raises:
    TypeError: on an unrecognized type of model.
    ValueError: if the model URI or other key parameters are not defined.
  """
  if not isinstance(llm, custom_model.CustomModel):
    raise TypeError("Unrecognized type of model.")

  # TODO(b/295042550): Add support other types of models (native to LangChain).

  if not llm.uri:
    raise ValueError("Model URI is required.")
  return pb.Computation(
      model_call=pb.ModelCall(model_id=pb.ModelId(uri=llm.uri))
  )


def create_computation_from_chain(chain):
  """Creates a `Computation` proto instance from a LangChain chain.

  Args:
    chain: An instance of LLMChain object.

  Returns:
    A corresponding instance of `pb.Computation`.
  """
  del chain
  # TODO(b/295042499): Implement this.
  return pb.Computation()


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
  if isinstance(langchain_object, langchain.chains.llm.LLMChain):
    return create_computation_from_chain(langchain_object)
  raise TypeError(
      "Unrecognized type of LangChain object {}.".format(
          str(type(langchain_object))
      )
  )
