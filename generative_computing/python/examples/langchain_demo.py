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
"""A simple example that shows how to execute some code based on LangChain."""

from collections.abc import Sequence
from absl import app
from langchain import chains
from langchain import prompts
from generative_computing.python import interop


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError("Too many command-line arguments.")

  my_chain = chains.LLMChain(
      llm=interop.langchain.CustomModel(uri="some_model"),
      prompt=prompts.PromptTemplate(
          input_variables=["location"],
          template="Q: What should I pack for a trip to {location}? A: ",
      ),
  )

  comp = interop.langchain.create_computation(my_chain)

  # Here we will create a runtime instance, and pass `comp` to it along
  # with the "location" argument to execute.
  del comp


if __name__ == "__main__":
  app.run(main)