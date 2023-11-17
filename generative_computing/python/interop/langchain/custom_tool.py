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
"""A class that represents an arbitrary tool for use with LangChain APIs."""

from typing import Any
from langchain import tools
from generative_computing.python.interop.langchain import custom_chain


class CustomTool(tools.BaseTool):
  """Represents an arbitrary tool for use with LangChain APIs."""

  name: str
  """A unique name for the tool that clearly communicates its purpose."""

  description: str
  """Describes the tool usage."""

  computation: custom_chain.CustomChain
  """The computation that the tool needs to run."""

  def _run(
      self,
      *args: Any,
      **kwargs: Any,
  ) -> Any:
    raise RuntimeError(
        "This is a tool placeholder, not to be executed directly."
    )
