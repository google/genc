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


_OPENAI_API_KEY = flags.DEFINE_string(
    "openai_api_key", None, "API key", required=True
)


def create_openai_request(
    api_key: str, endpoint: str, json_request: bytes
) -> pb.Value:
  request = pb.Value()
  args = request.struct
  args.element.add(label="api_key", str=api_key)
  args.element.add(label="endpoint", str=endpoint)
  args.element.add(label="json_request", str=json_request)

  return request


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError("Too many command-line arguments.")
  # send the json request in bytes only
  sample_request = (
      b'{"model": "gpt-3.5-turbo", "messages": [{"role": "user", "content":'
      b' "Can you help me know things to do in San Fransisco"}]}'
  )
  endpoint = "https://api.openai.com/v1/chat/completions"

  openai_arg = create_openai_request(
      api_key=_OPENAI_API_KEY.value,
      endpoint=endpoint,
      json_request=sample_request,
  )
  model_call = authoring.create_model("/openai/chatgpt")
  comp = runtime.Runner(comp_pb=model_call)
  result = json.loads(comp(openai_arg))
  if result.get("error"):
    print(result.get("error"))
  else:
    print(result["choices"][0]["message"]["content"])


if __name__ == "__main__":
  app.run(main)
