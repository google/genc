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
"""Tracing decorator for authoring computations."""

import inspect
from generative_computing.cc.authoring import constructor_bindings
from generative_computing.proto.v0 import computation_pb2 as pb
from generative_computing.python import base
from generative_computing.python.authoring import tracing_context


def traced_computation(fn):
  """Decorator that builds a portable computation by tracing a Python function.

  Args:
    fn: The function to trace.

  Returns:
    An instance of `Computation`
  """
  sig = inspect.signature(fn)
  params = sig.parameters.values()
  if not params:
    raise NotImplementedError('At least one parameter is required.')
  for param in params:
    if param.kind != param.POSITIONAL_OR_KEYWORD:
      raise NotImplementedError('Only positional parameters are supported.')
    if param.default != inspect.Parameter.empty:
      raise NotImplementedError('Default values are not supported.')
  my_context = tracing_context.TracingContext()
  base.context_stack.append_nested_context(my_context)
  try:
    arg = constructor_bindings.create_reference('arg')
    if len(params) > 1:
      arg_list = []
      for index, _ in enumerate(params):
        arg_list.append(constructor_bindings.create_selection(arg, index))
      result = fn(*arg_list)
    else:
      result = fn(arg)
  finally:
    base.context_stack.remove_nested_context(my_context)
  my_locals = my_context.locals
  if my_locals:
    result = pb.Value(
        block=pb.Block(
            local=[
                pb.Block.Local(name=k, value=v) for k, v in my_locals.items()
            ],
            result=result,
        )
    )
  portable_ir = constructor_bindings.create_lambda('arg', result)
  return base.Computation(portable_ir)
