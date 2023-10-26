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

from generative_computing.cc.intrinsics import intrinsic_bindings
from generative_computing.proto.v0 import computation_pb2 as pb


def create_model(model_uri):
  """Creates a model computation with the given model URI.

  Args:
    model_uri: The URI of the model.

  Returns:
    A computation that represents the model.
  """
  return pb.Value(
      intrinsic=pb.Intrinsic(
          uri=intrinsic_bindings.intrinsics.MODEL_INFERENCE,
          static_parameter=pb.Value(str=model_uri),
      )
  )


def create_prompt_template(template_str):
  """Creates a prompt template computation with the given template string.

  Args:
    template_str: The (Python-style) template string.

  Returns:
    A computation that represents the prompt template.
  """
  return pb.Value(
      intrinsic=pb.Intrinsic(
          uri=intrinsic_bindings.intrinsics.PROMPT_TEMPLATE,
          static_parameter=pb.Value(str=template_str),
      )
  )


def create_regex_partial_match(pattern_string):
  """Creates a regular expression partial match with the given pattern.

  Args:
    pattern_string: Pattern to search for in the input string.

  Returns:
    A computation that represents the regex partial match intrinsic.
  """
  return pb.Value(
      intrinsic=pb.Intrinsic(
          uri=intrinsic_bindings.intrinsics.REGEX_PARTIAL_MATCH,
          static_parameter=pb.Value(str=pattern_string),
      )
  )


def create_reference(name):
  """Constructs a reference to `name`.

  Args:
    name: The name of the referenced parameter.

  Returns:
    A computation that represents the reference; this computation can only be
    correctly evaluated as a part of a lambda expression or similar construct
    in which this name is well-defined.
  """
  return pb.Value(reference=pb.Reference(name=name))


def create_lambda(name, body):
  """Constructs a lambda expression with parameter `name` and body `body`.

  Args:
    name: The name of the parameter.
    body: The body of the lambda (with a reference to this parameter).

  Returns:
    A computation that represents the lambda expression.
  """
  return pb.Value(**{'lambda': pb.Lambda(parameter_name=name, result=body)})


def create_repeat(num_steps, body_fn):
  """Constructs a while loop, stop_fn is evaluated before entering the loop.

  Args:
    num_steps: Number of steps to repeat the calculation.
    body_fn: The loop body funciton name.

  Returns:
    A computation that represents the while loop.
  """
  elements = [
      pb.NamedValue(name='num_steps', value=pb.Value(str=str(num_steps))),
      pb.NamedValue(name='body_fn', value=pb.Value(str=body_fn)),
  ]

  return pb.Value(
      intrinsic=pb.Intrinsic(
          uri=intrinsic_bindings.intrinsics.REPEAT,
          static_parameter=pb.Value(struct=pb.Struct(element=elements)),
      )
  )


def create_call(fn, arg):
  """Constructs a function call.

  Args:
    fn: The function.
    arg: The argument.

  Returns:
    A computation that represents the function call.
  """
  return pb.Value(call=pb.Call(function=fn, argument=arg))


def create_struct(comp_list):
  """Constructs a struct.

  Args:
    comp_list: The list of elements.

  Returns:
    A computation that represents the struct.
  """
  elements = []
  for comp_pb in comp_list:
    elements.append(pb.NamedValue(value=comp_pb))
  return pb.Value(struct=pb.Struct(element=elements))


def create_selection(source, index):
  """Constructs a selection.

  Args:
    source: The source struct to select from.
    index: The index of the selected element.

  Returns:
    A computation that represents the selection.
  """
  return pb.Value(selection=pb.Selection(source=source, index=index))


def create_chain(function_list):
  """Assembles the given list of functions [f, g, ....] into a chain f(g(...)).

  Args:
    function_list: A list of functions [f, g, ...], each of which should be an
      instance of pb.Value.

  Returns:
    A computation that represents a chain f(g(...)), with the functions applied
    to the input in the order opposite to the order on the supplied list.
  """
  parameter_name = 'arg'
  arg = create_reference(parameter_name)
  for fn_pb in reversed(function_list):
    arg = create_call(fn_pb, arg)
  return create_lambda(parameter_name, arg)


def create_fallback(function_list):
  """Contructs a fallback expression from a given list of functions.

  Args:
    function_list: Candidate functions to attempt to apply to the argument in
      the order listed. The first successful one is the result; if failed, keep
      going down the list. All functions must be of type `pb.Value`.

  Returns:
    A computation that represents a fallback expression.
  """
  static_parameters = []
  for comp in function_list:
    static_parameters.append(pb.NamedValue(name='candidate_fn', value=comp))
  return pb.Value(
      intrinsic=pb.Intrinsic(
          uri=intrinsic_bindings.intrinsics.FALLBACK,
          static_parameter=pb.Value(
              struct=pb.Struct(element=static_parameters)
          ),
      )
  )


def create_conditional(condition, positive_branch, negative_branch):
  """Contructs a conditional expression.

  Conditional epxression picks among two alternatives based on a Boolean
  input, and only lazily evaluates of the two.

  Args:
    condition: Computation that evalutes to a Boolean result.
    positive_branch: Computation to be evaluated if the condition is true.
    negative_branch: Computation to be evaluated if the condition is false.

  Returns:
    A computation that represents the conditional expression.
  """
  conditional = pb.Value(
      intrinsic=pb.Intrinsic(
          uri=intrinsic_bindings.intrinsics.CONDITIONAL,
          static_parameter=pb.Value(
              struct=pb.Struct(
                  element=[
                      pb.NamedValue(name='then', value=positive_branch),
                      pb.NamedValue(name='else', value=negative_branch),
                  ]
              )
          ),
      )
  )
  return create_call(conditional, condition)
