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
"""A class that represents an arbitrary custom chain for use with LangChain APIs."""

from typing import Any, Dict, List, Optional, TypeAlias, Union

from langchain import chains
from langchain.callbacks import manager

from generative_computing.proto.v0 import computation_pb2 as pb

ChainedOpType: TypeAlias = Union[
    int, pb.Value, chains.base.Chain, chains.LLMChain, Any
]


class CustomChain(chains.base.Chain):
  """Represents an arbitrary custom chain for use with LangChain APIs."""

  # Enables mix-n-match to extend the versatility of chain.
  # 1. chains can be nested, this generalizes the chain definition
  # 2. different types of chains can be mixed, GenC will make them compatible.
  chained_ops: List[ChainedOpType] = []
  num_iteration: int = 0

  def __or__(self, op: ChainedOpType):
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
