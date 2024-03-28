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
"""Base classes for defining and managing `Computation` callables."""

from genc.proto.v0 import computation_pb2 as pb
from genc.python.base import context


class Computation(object):
  """Represents a computation internally expressed in terms of portable IR."""

  def __init__(self, portable_ir: pb.Value):
    self._portable_ir = portable_ir

  @property
  def portable_ir(self) -> pb.Value:
    return self._portable_ir

  def __call__(self, *args, **kwargs):
    ctx = context.context_stack.current_context
    if not ctx:
      raise RuntimeError('There is no current context to handle the call.')
    return ctx.call(self._portable_ir, *args, **kwargs)
