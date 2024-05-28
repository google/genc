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
"""Base classes for defining and managing contexts."""

import abc
from typing import Optional
from genc.proto.v0 import computation_pb2 as pb


class Context(metaclass=abc.ABCMeta):
  """An abstract interface to be implemented by all types of contexts."""

  @abc.abstractmethod
  def call(self, portable_ir: pb.Value, *args, **kwargs):
    """Synchronously process a call to `call_target` in this context.

    Args:
      portable_ir: The portable IR that's the computation function being called.
      *args: Positional args.
      **kwargs: Keyword args.

    Returns:
      The type of the result is context-dependent.
    """
    raise NotImplementedError

  @abc.abstractmethod
  def embed(self, arg):
    """Embeds `arg` for use in this context.

    Args:
      arg: The object (e.g., portable IR) to be embedded.

    Returns:
      The type of the result is context-dependent.
    """
    raise NotImplementedError


class ContextStack(object):
  """The global, shared stack of contexts."""

  def __init__(self):
    self._default_context = None
    self._nested_contexts = []

  def set_default_context(self, ctx: Context) -> None:
    """Makes `ctx` the default context, replacing whatever was current default.

    Args:
      ctx: An instance of `Context` to become the default.
    """
    self._default_context = ctx

  def append_nested_context(self, ctx: Context) -> None:
    """Appends `ctx` as the top of the context stack.

    Args:
      ctx: An instance of `Context` to append at the top of the stack.
    """
    self._nested_contexts.append(ctx)

  def remove_nested_context(self, ctx: Context) -> None:
    """Removes `ctx` from the top of the context stack.

    Args:
      ctx: An instance of `Context` to remove from the top of the stack.

    Raises:
      RuntimeError: if `ctx` is not at the top of the stack.
    """
    if not self._nested_contexts:
      raise RuntimeError('There is no nested context currently on the stack.')
    if self._nested_contexts[-1] != ctx:
      raise RuntimeError('Only a current non-default context can be removed.')
    self._nested_contexts.pop()

  @property
  def current_context(self) -> Optional[Context]:
    """Returns the context at the top of the stack (or default one if none)."""
    if self._nested_contexts:
      return self._nested_contexts[-1]
    else:
      return self._default_context


context_stack = ContextStack()
