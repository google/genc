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
"""A class that represents an arbitrary custom chain for use with LangChain APIs."""

from typing import Any, Dict, List, Optional, TypeAlias, Union

from langchain_core.callbacks import manager

from genc.proto.v0 import computation_pb2 as pb

# pylint: disable=g-import-not-at-top
try:
  from langchain_classic import chains
  from langchain_classic.chains import base as chains_base
except ImportError:  # langchain<1.0: OSS requirements.txt pins 0.1.0
  from langchain import chains  # pyrefly: ignore[missing-import, missing-module-attribute]
  from langchain.chains import base as chains_base  # pyrefly: ignore[missing-import]
# pylint: enable=g-import-not-at-top

ChainedOpType: TypeAlias = Union[
    int, pb.Value, chains_base.Chain, chains.LLMChain, Any
]


class CustomChain(chains.base.Chain):  # pyrefly: ignore[invalid-inheritance]
  """Represents an arbitrary custom chain for use with LangChain APIs."""

  # Enables mix-n-match to extend the versatility of chain.
  # 1. chains can be nested, this generalizes the chain definition
  # 2. different types of chains can be mixed, GenC will make them compatible.
  chained_ops: List[ChainedOpType] = []
  num_iteration: int = 0

  def __or__(self, op: ChainedOpType):  # pyrefly: ignore[bad-override]
    if isinstance(op, int):
      self.num_iteration = op
    else:
      self.chained_ops.append(op)
    return self

  @property
  def input_keys(self) -> List[str]:
    raise RuntimeError("This is a chain placeholder. keys are not needed")

  @property
  def output_keys(self) -> List[str]:
    raise RuntimeError("This is a chain placeholder. keys are not needed")

  def _call(
      self,
      inputs: Dict[str, Any],
      run_manager: Optional[manager.CallbackManagerForChainRun] = None,
  ) -> Dict[str, str]:
    raise RuntimeError(
        "This is a chain placeholder, not to be executed directly."
    )

  @property
  def _chain_type(self) -> str:
    return "genc_custom_chain"
