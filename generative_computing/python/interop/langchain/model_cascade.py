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
"""A class that represents a model cascade for use with LangChain APIs."""

from typing import Any, List, Mapping, Optional
from langchain.callbacks.manager import CallbackManagerForLLMRun  # pylint:disable=g-importing-member
from langchain.llms.base import LLM  # pylint:disable=g-importing-member


class ModelCascade(LLM):
  """Represents a model cascade for use with LangChain APIs."""

  models: List[LLM] = []

  @property
  def _llm_type(self) -> str:
    return "ModelCascade({})".format(
        ", ".join([x._llm_type for x in self.models])  # pylint: disable=protected-access
    )

  def _call(
      self,
      prompt: str,
      stop: Optional[List[str]] = None,
      run_manager: Optional[CallbackManagerForLLMRun] = None,
      **kwargs: Any
  ) -> str:
    raise RuntimeError(
        "This is a model cascade placeholder, not to be executed directly."
    )

  @property
  def _identifying_params(self) -> Mapping[str, Any]:
    """Get the identifying parameters."""
    del self
    return {}
