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
"""Tracing context for authoring."""

from genc.cc.authoring import constructor_bindings
from genc.proto.v0 import computation_pb2 as pb
from genc.python import base


class TracingContext(base.Context):
  """A tracing context for invocations of the `Computation` class."""

  def __init__(self):
    self._name_to_ir = {}
    self._ir_str_to_name = {}

  def call(self, portable_ir: pb.Value, *args, **kwargs):
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
    if not isinstance(ir, pb.Value):
      ir = base.to_value_proto(ir)
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
