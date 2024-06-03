# Copyright 2024, The GenC Authors.
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
"""Utility functions used in LlamaCpp interop."""


def create_config(model_path, **kwargs):
  """Creates a config for LlamaCpp.

  Args:
    model_path: The path to the model.
    **kwargs: Optional keyword arguments to pass to LlamaCpp, such as the
      number of threads or max number of tokens.

  Returns:
    An instance of the `dict` object.
  """
  config = {"model_path": model_path}
  config.update(kwargs)
  return config
