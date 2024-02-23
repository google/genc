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
"""Library for getting demo executor."""

from generative_computing.cc.examples.executors import executor_bindings


def create_default_executor():
  """Creates a default executor used in generative computing demos.

  Returns:
    An executor that contains handlers for generative computing demos.
  """
  return executor_bindings.create_default_executor()
