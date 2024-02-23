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

"""LangChain Agent demo - combining LangChain Interface with GenC C++ runtime."""

from collections.abc import Sequence
import textwrap
from absl import app
from absl import flags
import langchain
from langchain import prompts
import generative_computing as genc
from generative_computing.python.examples import executor

_APPID = flags.DEFINE_string(
    "appid", None, "appid of WolframAlpha", required=True
)
_API_KEY = flags.DEFINE_string(
    "api_key", None, "Gemini API Auth Token", required=True
)


class WolframAlpha(genc.interop.langchain.CustomTool):
  """A specific implementation of CustomTool, including an appid for identification."""

  def __init__(self, appid: str):
    name = "WolframAlpha"
    description = (
        "https://products.wolframalpha.com/api/documentation, one capability is"
        " to evaluate math expression."
    )
    # Extract math equation from a text, then calls WolfraAlpha
    computation = (
        genc.interop.langchain.CustomChain()
        | genc.authoring.create_custom_function("/react/extract_math_question")
        | genc.authoring.create_wolfram_alpha(appid)
        # Template Engine will extract the result from response JSON
        | genc.authoring.create_inja_template(
            "{% if queryresult.pods"
            " %}{{queryresult.pods.0.subpods.0.plaintext}}{% endif %}"
        )
    )

    super().__init__(
        name=name, description=description, computation=computation
    )


class MathToolAgent(genc.interop.langchain.CustomAgent):
  """An agent that combines ReAct reasoning loop with WolframAlpha tools."""

  def __init__(self, appid: str, api_key: str):

    system_instruction = textwrap.dedent("""
    Solve a question answering task with interleaving Thought, Action, Observation steps
    Thought can reason about the current situation
    Action can be only two types:
    (1) Math[query], Useful for when you need to solve math problems.
    (2) Finish[answer], which returns the answer as a number terminates.
    Here's an example:
    Question: what is the result of power of 2 + 1?
    Thought: power of 2
    Action: Math[pow(2)]
    Observation: 4
    Thought: I can now process the answer.
    Action: Math[4+1]
    Observation: 5
    Thought: Seems like I got the answer.
    Action: Finish[5]
    Please do it step by step.
    Question: {question}""")

    prompt = prompts.PromptTemplate(
        input_variables=["question"], template=system_instruction
    )
    print(appid)
    tools = [WolframAlpha(appid)]

    llm = genc.interop.langchain.CustomModel(
        uri="/cloud/gemini",
        config=genc.interop.gemini.create_config(api_key),
    )
    # Init langchain.agents.agent.Agent with components backed by C++ runtime.
    super().__init__(
        llm_chain=langchain.chains.LLMChain(llm=llm, prompt=prompt),
        allowed_tools=[tool.name for tool in tools],
        tools_list=tools,
        max_iterations=8,
    )


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError("Too many command-line arguments.")

  agent = MathToolAgent(_APPID.value, _API_KEY.value)

  # IR stands for intermediate representation.
  # It transforms the agent into a computation,
  # Which can be ported to C++, Android.
  portable_ir = genc.interop.langchain.create_computation(agent)
  # To run it localy
  runner = genc.runtime.Runner(portable_ir, executor.create_default_executor())
  result = runner(
      "what is the result of (square root of 4) + 3 to the power of 2 + 3 *(8 +"
      " 4) / 2 - 7"
  )
  print(result)


if __name__ == "__main__":
  app.run(main)
