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
"""Library for getting demo executor."""

from genc.cc.examples.executors import executor_bindings
from genc.python import base
from genc.python import runtime


def create_default_executor():
  """Creates a default executor used in GenC demos.

  Returns:
    An executor that contains handlers for GenC demos.
  """
  return executor_bindings.create_default_executor()


def set_default_executor():
  base.context_stack.set_default_context(
      runtime.SynchronousContext(create_default_executor()))
