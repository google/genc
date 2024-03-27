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
"""Libraries for constructing computations."""

from generative_computing.cc.authoring import constructor_bindings


# TODO(b/307573292): deprecate python constructors available via C++ bindings.
def create_model(model_uri):
  """Creates a model computation with the given model URI.

  Args:
    model_uri: The URI of the model.

  Returns:
    A computation that represents the model.
  """
  return constructor_bindings.create_model(model_uri)


# TODO(b/325090417): merge into CreateModelInference with nullable config.
def create_model_with_config(model_uri, model_config):
  """Creates a model computation with the given model URI and model config.

  Args:
    model_uri: The URI of the model.
    model_config: The config for the model.

  Returns:
    A computation that represents the model.
  """
  return constructor_bindings.create_model_with_config(model_uri, model_config)


def create_rest_model_config(endpoint, api_key=""):
  """Creates a model computation with the given model URI and model config.

  Args:
    endpoint: The REST endpoint of the model.
    api_key: An optional API key.

  Returns:
    A value that represents the model config. That can be used in conjunction
    with create_model_with_config.
  """
  return constructor_bindings.create_rest_model_config(endpoint, api_key)


def create_prompt_template(template_str):
  """Creates a prompt template computation with the given template string.

  Args:
    template_str: The (Python-style) template string.

  Returns:
    A computation that represents the prompt template.
  """
  return constructor_bindings.create_prompt_template(template_str)


def create_regex_partial_match(pattern_string):
  """Creates a regular expression partial match with the given pattern.

  Args:
    pattern_string: Pattern to search for in the input string.

  Returns:
    A computation that represents the regex partial match intrinsic.
  """
  return constructor_bindings.create_regex_partial_match(pattern_string)


def create_reference(name):
  """Constructs a reference to `name`.

  Args:
    name: The name of the referenced parameter.

  Returns:
    A computation that represents the reference; this computation can only be
    correctly evaluated as a part of a lambda expression or similar construct
    in which this name is well-defined.
  """
  return constructor_bindings.create_reference(name)


def create_lambda(name, body):
  """Constructs a lambda expression with parameter `name` and body `body`.

  Args:
    name: The name of the parameter.
    body: The body of the lambda (with a reference to this parameter).

  Returns:
    A computation that represents the lambda expression.
  """
  return constructor_bindings.create_lambda(name, body)


def create_lambda_from_fn(parameter_name, body_fn):
  """Constructs a lambda expression from a parameter and one-argument function.

  Constructs a reference named `parameter_name`, and feeds it as an argument
  to function `body_fn` to produce the result computation, then packages both
  as a lambda.

  Args:
    parameter_name: The name of the parameter.
    body_fn: A function that constructs the body of the lambda from parameter
      reference computation fed at input.

  Returns:
    A computation that represents the lambda expression.
  """
  reference = constructor_bindings.create_reference(parameter_name)
  body = body_fn(reference)
  return constructor_bindings.create_lambda(parameter_name, body)


def create_repeat(num_steps, body_fn):
  """Constructs a while loop, stop_fn is evaluated before entering the loop.

  Args:
    num_steps: Number of steps to repeat the calculation.
    body_fn: The loop body funciton name.

  Returns:
    A computation that represents the while loop.
  """
  return constructor_bindings.create_repeat(num_steps, body_fn)


def create_call(fn, arg):
  """Constructs a function call.

  Args:
    fn: The function.
    arg: The argument.

  Returns:
    A computation that represents the function call.
  """
  return constructor_bindings.create_call(fn, arg)


def create_struct(comp_list):
  """Constructs a struct.

  Args:
    comp_list: The list of elements.

  Returns:
    A computation that represents the struct.
  """
  return constructor_bindings.create_struct(comp_list)


def create_selection(source, index):
  """Constructs a selection.

  Args:
    source: The source struct to select from.
    index: The index of the selected element.

  Returns:
    A computation that represents the selection.
  """
  return constructor_bindings.create_selection(source, index)


def create_named_value(name, value):
  """Constructs a named value.

  Args:
    name: The name of the named value.
    value: The value of the named value.

  Returns:
    A NamedValue.
  """
  return constructor_bindings.create_named_value(name, value)


def create_fallback(function_list):
  """Contructs a fallback expression from a given list of functions.

  Args:
    function_list: Candidate functions to attempt to apply to the argument in
      the order listed. The first successful one is the result; if failed, keep
      going down the list. All functions must be of type `pb.Value`.

  Returns:
    A computation that represents a fallback expression.
  """
  return constructor_bindings.create_fallback(function_list)


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
  return constructor_bindings.create_conditional(
      condition, positive_branch, negative_branch
  )


def create_while(condition_fn, body_fn):
  """Constructs a while loop.

  Args:
    condition_fn: The condition function. The condition function controls while
      loop iterations. If condition function evaluates to false, the while loop
      is terminated. If it evaluates to true, while loop continues with the next
      iteration.
    body_fn: The loop body function.

  Returns:
    A computation that represents the while loop.
  """
  return constructor_bindings.create_while(condition_fn, body_fn)


def create_serial_chain(function_list):
  """Constructs a serial chain expression.

  Args:
    function_list: A list of functions.

  Returns:
    A computation that represents the serial chain expression.
  """
  return constructor_bindings.create_serial_chain(function_list)


def create_breakable_chain(function_list):
  """Constructs a breakable chain expression.

  Args:
    function_list: A list of functions.

  Returns:
    A computation that represents a breakable chain expression.
  """
  return constructor_bindings.create_breakable_chain(function_list)


def create_custom_function(fn_uri):
  """Constructs a custom function expression.

  Args:
    fn_uri: The URI of the custom function.

  Returns:
    A computation that represents a custom function expression.
  """
  return constructor_bindings.create_custom_function(fn_uri)


def create_wolfram_alpha(appid):
  """Constructs a call to WolframAlpha.

  Args:
    appid: appid for WolframAlpha.

  Returns:
    A computation that represents a call to WolframAlpha.
  """
  return constructor_bindings.create_wolfram_alpha(appid)


def create_rest_call(uri, api_key="", method="POST"):
  """Constructs a rest call.

  Args:
    uri: The REST endpoint URI.
    api_key: The API key. Empty string if API is embedded into URI.
    method: currently supports POST, GET

  Returns:
    A computation that represents rest call.
  """
  return constructor_bindings.create_rest_call(uri, api_key, method)


def create_logger():
  """Constructs a logger expression.

  Returns:
    A computation that represents a logger expression.
  """
  return constructor_bindings.create_logger()


def create_logical_not():
  """Constructs a logical not expression.

  Returns:
    A computation that represents a logical not expression.
  """
  return constructor_bindings.create_logical_not()


def create_inja_template(template):
  """Constructs a Inja Template.

  Args:
    template: string representing the template.

  Returns:
    A computation that represents a Inja Template.
  """
  return constructor_bindings.create_inja_template(template)


def create_repeated_conditional_chain(num_steps, function_list):
  """Constructs a repeated conditionalchain expression.

  Args:
    num_steps: Number of steps to repeat the calculation.
    function_list: A list of functions.

  Returns:
    A computation that represents a typical reasoning loop expression.
  """
  return constructor_bindings.create_repeated_conditional_chain(
      num_steps, function_list
  )


def create_parallel_map(map_fn):
  """Constructs a parallel map expression.

  Args:
    map_fn: The map function to be applied to all input values.

  Returns:
    A computation that represents a parallel map expression.
  """
  return constructor_bindings.create_parallel_map(map_fn)
