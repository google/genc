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
"""Libraries fo packing and unpacking to/from `Value` proto."""

from google3.google.protobuf import any_pb2
from google3.net.proto2.python.public import message
from genc.proto.v0 import computation_pb2 as pb
from genc.python.base import computation


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
      element = to_value_proto(value)
      element.label = key
      struct.element.append(element)
    return pb.Value(struct=struct)
  elif isinstance(arg, message.Message):
    # Create a new Any message
    any_message = any_pb2.Any()
    # Pack the input message into the Any message
    any_message.Pack(arg)
    # Create a Value message and set its any_val field
    value_message = pb.Value(custom_value=any_message)
    return value_message
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
  elif which_result == 'custom_value':
    return result_pb.custom_value
  elif which_result == 'struct':
    return [from_value_proto(x) for x in result_pb.struct.element]
  else:
    raise TypeError('Unsupported value proto type {}.'.format(which_result))
