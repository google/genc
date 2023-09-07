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
"""Libraries for constructing computations."""

from generative_computing.proto.v0 import computation_pb2 as pb


def create_model(model_uri):
  """Creates a model computation with the given model URI.

  Args:
    model_uri: The URI of the model.

  Returns:
    A computation that represents the model.
  """
  return pb.Computation(model=pb.Model(model_id=pb.ModelId(uri=model_uri)))


def create_prompt_template(template_str):
  """Creates a prompt template computation with the given template string.

  Args:
    template_str: The (Python-style) template string.

  Returns:
    A computation that represents the prompt template.
  """
  return pb.Computation(
      prompt_template=pb.PromptTemplate(template_string=template_str)
  )


def create_chain(function_list):
  """Assembles the given list of functions [f, g, ....] into a chain f(g(...)).

  Args:
    function_list: A list of functions [f, g, ...], each of which should be an
      instance of pb.Computation.

  Returns:
    A computation that represents a chain f(g(...)), with the functions applied
    to the input in the order opposite to the order on the supplied list.
  """
  parameter_name = 'arg'
  arg = pb.Computation(reference=pb.Reference(name=parameter_name))
  for fn_pb in reversed(function_list):
    arg = pb.Computation(call=pb.Call(function=fn_pb, argument=arg))
  return pb.Computation(
      **{'lambda': pb.Lambda(parameter_name=parameter_name, result=arg)}
  )
