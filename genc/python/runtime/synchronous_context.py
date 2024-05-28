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
"""Synchronous runtime context for ordinary execution."""

from genc.proto.v0 import computation_pb2 as pb
from genc.python import base
from genc.python.runtime import runner


class SynchronousContext(base.Context):
  """A context for synchronous execution of calls to `Computation`."""

  def __init__(self, executor):
    self._executor = executor

  def call(self, portable_ir: pb.Value, *args, **kwargs):
    """Processes the call to `portable_ir` in this runtime context.

    Args:
      portable_ir: The IR being called.
      *args: Positional args.
      **kwargs: Keyword args.

    Returns:
      The synchronously computed result of this (blocking) call.
    """
    return runner.Runner(portable_ir, self._executor)(*args, **kwargs)

  def embed(self, arg):
    raise NotImplementedError('Not currently supported in a runtime context.')


def set_default_executor(executor=None):
  base.context_stack.set_default_context(SynchronousContext(executor))
