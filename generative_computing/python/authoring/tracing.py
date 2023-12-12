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
"""Tracing-based authoring libraries (under construction)."""

import inspect
from generative_computing.cc.authoring import constructor_bindings
from generative_computing.proto.v0 import computation_pb2 as pb


class AuthoringContext(object):
  """A tracing context for invocations of the `Computation` class."""

  def __init__(self):
    self._name_to_ir = {}
    self._ir_str_to_name = {}

  def call(self, portable_ir, *args, **kwargs):
    """Processed the call to `portable_ir` in this authoring context.

    Args:
      portable_ir: The IR being called.
      *args: Positional args.
      **kwargs: Keyword args.

    Returns:
      The IR that represents the result.
    """
    if kwargs:
      raise NotImplementedError('Only positional arguments are supported.')
    if not args:
      raise NotImplementedError('At least one argument is required.')
    processed_args = [self._replace_with_reference(x) for x in args]
    if len(processed_args) > 1:
      input_val = constructor_bindings.create_struct(processed_args)
    else:
      input_val = processed_args[0]
    processed_func = self._replace_with_reference(portable_ir)
    return constructor_bindings.create_call(processed_func, input_val)

  @property
  def locals(self):
    return self._name_to_ir

  def _replace_with_reference(self, ir):
    if ir.WhichOneof('value') == 'reference':
      return ir
    ir_str = str(ir)
    if ir_str in self._ir_str_to_name:
      name = self._ir_str_to_name[ir_str]
    else:
      name = 'v_{}'.format(len(self._name_to_ir))
      self._ir_str_to_name[ir_str] = name
      self._name_to_ir[name] = ir
    return constructor_bindings.create_reference(name)


context_stack = []


class Computation(object):
  """Represents a portable computation constructed as a result of tracing."""

  def __init__(self, fn):
    sig = inspect.signature(fn)
    params = sig.parameters.values()
    if not params:
      raise NotImplementedError('At least one parameter is required.')
    for param in params:
      if (param.kind != param.POSITIONAL_OR_KEYWORD):
        raise NotImplementedError('Only positional parameters are supported.')
      if (param.default != inspect.Parameter.empty):
        raise NotImplementedError('Default values are not supported.')
    if context_stack:
      raise NotImplementedError('Embedded functions are not yet supported.')
    my_context = AuthoringContext()
    context_stack.append(my_context)
    arg = constructor_bindings.create_reference('arg')
    if len(params) > 1:
      arg_list = []
      for index, _ in enumerate(params):
        arg_list.append(constructor_bindings.create_selection(arg, index))
      result = fn(*arg_list)
    else:
      result = fn(arg)
    my_locals = my_context.locals
    context_stack.clear()
    if my_locals:
      result = pb.Value(block=pb.Block(
          local=[pb.Block.Local(name=k, value=v) for k, v in my_locals.items()],
          result=result))
    self._portable_ir = constructor_bindings.create_lambda('arg', result)

  @property
  def portable_ir(self):
    return self._portable_ir

  def __call__(self, *args, **kwargs):
    if not context_stack:
      raise NotImplementedError('Calls are supported only while tracing.')
    current_context = context_stack[0]
    return current_context.call(self._portable_ir, *args, **kwargs)


def computation(fn):
  """Decorator that builds a portable computation by tracing a Python function.

  Args:
    fn: The function to trace.

  Returns:
    An instance of `Computation`
  """
  return Computation(fn)
