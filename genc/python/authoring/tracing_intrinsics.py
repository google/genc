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
"""Tracing support for intrinsics."""

from genc.cc.intrinsics import intrinsic_bindings
from genc.python.authoring.computation_from_intrinsic import ComputationFromIntrinsic


# pylint:disable=invalid-name
# pylint:disable=redefined-outer-name


for x in dir(intrinsic_bindings.intrinsics):
  if not x.startswith('__'):
    name = getattr(intrinsic_bindings.intrinsics, x)
    comp = ComputationFromIntrinsic(name)
    globals()[name] = comp


__all__ = list(globals().keys())


def __getattr__(name):
  if name in __all__:
    return globals()[name]
  else:
    raise AttributeError
