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
"""An example shows how to combine tools with reasoning."""

from collections.abc import Sequence
import textwrap

from absl import app
from absl import flags

import genc as genc
from genc.python.examples import executor


# An example agent that combines reasoning loop with WolframAlpha as tool.
# For production better prompt engineering is needed.
# This example uses GenC's native authoring interfaces.

_APPID = flags.DEFINE_string(
    "appid", None, "appid of WolframAlpha", required=True
)
_API_KEY = flags.DEFINE_string(
    "api_key", None, "Gemini API Auth Token", required=True
)


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError("Too many command-line arguments.")

  insutrction_template = textwrap.dedent("""
    Solve a question answering task with interleaving Thought, Action, Observation steps.
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
    Question: {question}
    """)

  # Logger prints to terminal
  log_it = genc.authoring.create_logger()

  # Context keeps the interaction history with model.
  # These Context is backed by a thread safe key value store.
  read_context = genc.authoring.create_custom_function("/local_cache/read")
  add_to_context = genc.authoring.create_custom_function("/local_cache/write")
  evict_context = genc.authoring.create_custom_function("/local_cache/remove")

  model_config = genc.authoring.create_rest_model_config(
      "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent",
      _API_KEY.value,
  )
  model_call = genc.authoring.create_model_with_config(
      "/cloud/gemini", model_config
  )

  # Use WolframAlpha as a Tool to solve simple math questions.
  solve_math = (
      genc.interop.langchain.CustomChain()
      | genc.authoring.create_custom_function("/react/extract_math_question")
      | genc.authoring.create_wolfram_alpha(_APPID.value)
      # Template Engine will extract the result from response JSON
      | genc.authoring.create_inja_template(
          "{% if queryresult.pods"
          " %}{{queryresult.pods.0.subpods.0.plaintext}}{% endif %}"
      )
  )

  # Define a resoning loop
  # It will keep executing until a boolean op in the chain evaluates to true.
  reasoning_loop = (
      genc.interop.langchain.CustomChain()
      | read_context
      | model_call
      | genc.authoring.create_custom_function("/react/parse_thought_action")
      | log_it
      | genc.authoring.create_regex_partial_match("Finish")
      | add_to_context
      | solve_math
      | genc.authoring.create_custom_function("/react/format_observation")
      | log_it
      | add_to_context
  )
  # If agent can't get an answer by 8th iteration, terminate.
  reasoning_loop.num_iteration = 8

  # Now set up the instruction tempalte and wire the agent together.
  instruction_template = genc.authoring.create_prompt_template(
      insutrction_template
  )

  math_agent_chain = (
      genc.interop.langchain.CustomChain()
      | evict_context
      | instruction_template
      | add_to_context
      | log_it
      | reasoning_loop
  )

  portable_ir = genc.interop.langchain.create_computation(math_agent_chain)
  runner = genc.runtime.Runner(portable_ir, executor.create_default_executor)
  runner(
      "what is the result of (square root of 4) + (3 to the power of 2) + 3 *(8"
      " + 4) / 2 - 7"
  )


if __name__ == "__main__":
  app.run(main)
