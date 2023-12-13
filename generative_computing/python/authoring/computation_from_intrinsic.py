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
"""Computation override for tracing support for intrinsics."""

from generative_computing.proto.v0 import computation_pb2 as pb
from generative_computing.python import base


class ComputationFromIntrinsic(base.Computation):
  """An override of `Computation` that accepts intrinsic static parameters."""

  def __init__(self, intrinsic_uri: str):
    self._intrinsic_uri = intrinsic_uri
    super().__init__(pb.Value(intrinsic=pb.Intrinsic(uri=self._intrinsic_uri)))

  def __getitem__(self, key):
    if not key:
      return ValueError('Expected at least one intrinsic static argument.')
    if isinstance(key, tuple):
      if len(key) > 1:
        static_arg = base.to_value_proto(list(key))
      else:
        static_arg = base.to_value_proto(key[0])
    else:
      static_arg = base.to_value_proto(key)
    portable_ir = pb.Value(
        intrinsic=pb.Intrinsic(
            uri=self._intrinsic_uri, static_parameter=static_arg
        )
    )
    return base.Computation(portable_ir)
