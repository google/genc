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
"""Utility function for creating `Computation` protos from LangChain objects."""


def create_computation(langchain_object):
  """Creates a `Computation` proto instance from a LangChain object.

  Args:
    langchain_object: An object (e.g., a chain) created using LangChain APIs.

  Returns:
    An instance of the `Computation` proto.
  """
  del langchain_object
