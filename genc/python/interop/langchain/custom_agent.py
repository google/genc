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
"""A class that represents a custom agent for use with LangChain APIs."""
from __future__ import annotations

from typing import Any, Dict, List, Optional, Sequence

from pydantic import v1 as pydantic_v1

from genc.python.interop.langchain import custom_tool

# pylint: disable=g-import-not-at-top
try:
  from langchain_classic import agents
  from langchain_classic import chains
except ImportError:  # langchain<1.0: OSS requirements.txt pins 0.1.0
  from langchain import agents  # pyrefly: ignore[missing-import]
  from langchain import chains  # pyrefly: ignore[missing-import, missing-module-attribute]
# pylint: enable=g-import-not-at-top


class CustomAgent(agents.Agent):  # pyrefly: ignore[invalid-inheritance, missing-attribute]
  """Represents an arbitrary agent for use with LangChain APIs."""

  # Part of Langchain's Agent API. Langchain validates that this is correctly
  # constructed.
  llm_chain: chains.LLMChain
  """The LLM chain to use in the agent."""

  # Part of Langchain's Agent API. Make it optional in GenC interop.
  output_parser: Optional[Any] = None
  """The output parser to use in the agent."""

  # Part of Langchain's Agent API.
  allowed_tools: List[str] = None  # pyrefly: ignore[bad-assignment]
  """The names of the tools that are allowed to be used."""

  # List of tools to use with this Agent.
  tools_list: List[custom_tool.CustomTool]
  """The list of tools allowed to be used."""

  # Maximum iterations of the reasoning loop allowed.
  max_iterations: int
  """The maximum depth of a reasoning chain."""

  return_intermediate_steps: bool = False
  """Whether to return the agent's trajectory of intermediate steps."""

  def _run(
      self,
      *unused_args: Any,
      **unused_kwargs: Any,
  ) -> Any:
    raise RuntimeError(
        "This is a tool placeholder, not to be executed directly."
    )

  @property
  def llm_prefix(self) -> str:
    raise RuntimeError("This is agent placeholder. llm_prefix not needed.")

  @property
  def observation_prefix(self) -> str:
    raise RuntimeError("This is a placeholder. observation_prefix not needed.")

  @classmethod
  def create_prompt(cls, tools: Sequence[Any]) -> Any:
    """Create a prompt for this class."""
    raise RuntimeError(
        "This is an agent placeholder. create_prompt not needed."
    )

  @classmethod
  def _get_default_output_parser(cls, **kwargs: Any) -> Any:
    """Get default output parser for this class."""
    raise RuntimeError(
        "This is an agent placeholder. _get_default_output_parser not needed."
    )

  @classmethod
  def _validate_tools(cls, tools: Sequence[Any]) -> None:
    """Validate that appropriate tools are passed in."""
    pass

  @pydantic_v1.root_validator()  # pyrefly: ignore[no-matching-overload]
  def validate_prompt(cls, values: Dict[Any, Any]) -> Dict[Any, Any]:  # pylint: disable=no-self-argument  # pyrefly: ignore[bad-override]
    """Validate that prompt matches format."""
    return values
