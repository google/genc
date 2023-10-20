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
"""Libraries fo constructing runners for executing generative computations."""

from generative_computing.cc.runtime import executor_bindings
from generative_computing.proto.v0 import computation_pb2 as pb


def _to_value_proto(arg):
  """Creates an executor value proto that corresponds to the given argument.

  Args:
    arg: A regular Python argument.

  Returns:
    A corresponding instance of `pb.Value`.

  Raises:
    TypeError: if the argument is of an unsupported type.
  """
  if isinstance(arg, pb.Value):
    return arg
  if isinstance(arg, str):
    return pb.Value(**{'str': arg})
  if isinstance(arg, bool):
    return pb.Value(boolean=arg)
  raise TypeError('Unsupported Python argument type {}.'.format(type(arg)))


# TODO(b/305092775): consider deprecate this.
def _from_value_proto(result_pb):
  """Creates a Python value object that corresponds to the given value proto.

  Args:
    result_pb: An instance of `pb.Value`.

  Returns:
    A corresponding Python object.

  Raises:
    TypeError: if the `result_pb` is of an unsupported type.
  """
  which_result = result_pb.WhichOneof('value')
  if not which_result:
    return None
  if which_result == 'str':
    return result_pb.str
  if which_result == 'boolean':
    return result_pb.boolean
  if which_result == 'media':
    return result_pb.media
  if which_result == 'struct':
    return result_pb.struct
  raise TypeError('Unsupported value proto type {}.'.format(which_result))


# TODO(b/305092775): deprecate Python Runner methods, provide C++ bindings.
class Runner(object):
  """Represents a runner."""

  def __init__(
      self, comp_pb: pb.Value, executor: executor_bindings.Executor = None
  ):
    """Construct a runner for the given computation proto.

    Args:
      comp_pb: The computation proto to contruct the runner for.
      executor: The executor to use for the computation. If not set, a new
        default local executor will be created.
    """
    if executor is None:
      self._executor = executor_bindings.create_default_local_executor()
    else:
      self._executor = executor
    self._comp_pb = comp_pb
    self._comp_val = self._executor.create_value(self._comp_pb)

  def __call__(self, *args, **kwargs):
    """Invoke the runner on a given set of arguments.

    This is a blocking call that performs the computation synchronously.

    Args:
      *args: Positonal args.
      **kwargs: Keyword args.

    Returns:
      The result of the computation, computed synchronously.
    """
    if kwargs:
      raise ValueError('Keyword arguments are not currently supported.')
    try:
      if not args:
        arg_val = None
      elif len(args) > 1:
        elements = [
            self._executor.create_value(_to_value_proto(x)) for x in args
        ]
        arg_val = self._executor.create_struct([x.ref for x in elements])
      else:
        arg_val = self._executor.create_value(_to_value_proto(args[0]))
      result_val = self._executor.create_call(
          self._comp_val.ref, arg_val.ref if arg_val else None
      )
      result_pb = self._executor.materialize(result_val.ref)
      return _from_value_proto(result_pb)
    except Exception as err:
      raise RuntimeError(str(err)) from err
