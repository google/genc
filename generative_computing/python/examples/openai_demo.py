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
import json

from absl import app
from absl import flags

from generative_computing.proto.v0 import computation_pb2 as pb
from generative_computing.python import authoring
from generative_computing.python import runtime

# An example that calls OpenAI as part of the computation.
# bazel run generative_computing/python/examples:openai_demo -- --api_key <key>

# Note: API key differs from ChatGPT subscription.
# For key follow https://platform.openai.com/docs/introduction


FLAGS = flags.FLAGS
flags.DEFINE_string("api_key", None, "API key for OpenAI", required=True)


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError("Too many command-line arguments.")

  endpoint = "https://api.openai.com/v1/chat/completions"

  model_call = authoring.create_rest_call(endpoint, FLAGS.api_key)
  comp = runtime.Runner(comp_pb=model_call)

  request = pb.Value()
  request.str = (
      b'{"model": "gpt-3.5-turbo", "messages": [{"role": "user", "content":'
      b' "Can you help me know things to do in San Fransisco"}]}'
  )

  result = json.loads(comp(request))

  if result.get("error"):
    print(result.get("error"))
  else:
    print(result["choices"][0]["message"]["content"])


if __name__ == "__main__":
  app.run(main)
