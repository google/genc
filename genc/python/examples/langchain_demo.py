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
"""A simple example that shows how to execute some code based on LangChain."""

from collections.abc import Sequence
from absl import app
from langchain import chains
from langchain import prompts
from genc.python import interop
from genc.python import runtime
from genc.python.examples import executor


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError("Too many command-line arguments.")

  my_chain = chains.LLMChain(
      llm=interop.langchain.CustomModel(uri="test_model"),
      prompt=prompts.PromptTemplate(
          input_variables=["location"],
          template="Q: What should I pack for a trip to {location}? A: ",
      ),
  )

  comp_pb = interop.langchain.create_computation(my_chain)
  comp = runtime.Runner(comp_pb, executor.create_default_executor())
  result = comp("a grocery store")
  print(result)


if __name__ == "__main__":
  app.run(main)
