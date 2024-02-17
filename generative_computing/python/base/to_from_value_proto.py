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
"""Libraries fo packing and unpacking to/from `Value` proto."""

from generative_computing.proto.v0 import computation_pb2 as pb
from generative_computing.python.base import computation


def to_value_proto(arg):
  """Creates a value proto that corresponds to the given Python argument.

  Args:
    arg: A regular Python argument.

  Returns:
    A corresponding instance of `pb.Value`.

  Raises:
    TypeError: if the argument is of an unsupported type.
  """
  if isinstance(arg, pb.Value):
    return arg
  elif isinstance(arg, computation.Computation):
    return arg.portable_ir
  elif isinstance(arg, str):
    return pb.Value(**{'str': arg})
  elif isinstance(arg, bool):
    return pb.Value(boolean=arg)
  elif isinstance(arg, int):
    return pb.Value(int_32=arg)
  elif isinstance(arg, float):
    return pb.Value(float_32=arg)
  elif isinstance(arg, list):
    return pb.Value(struct=pb.Struct(element=[to_value_proto(x) for x in arg]))
  elif isinstance(arg, dict):
    struct = pb.Struct()
    for key, value in arg.items():
      struct.element.append(pb.Value(label=key, str=value))
    return pb.Value(struct=struct)
  else:
    raise TypeError('Unsupported Python argument type {}.'.format(type(arg)))


def from_value_proto(result_pb):
  """Creates a Python value object that corresponds to the given `Value` proto.

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
  elif which_result == 'boolean':
    return result_pb.boolean
  elif which_result == 'int_32':
    return result_pb.int_32
  elif which_result == 'float_32':
    return result_pb.float_32
  elif which_result == 'struct':
    return [from_value_proto(x) for x in result_pb.struct.element]
  else:
    raise TypeError('Unsupported value proto type {}.'.format(which_result))
