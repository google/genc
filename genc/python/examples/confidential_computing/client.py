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
"""An example of confidential computing in Python."""

from collections.abc import Sequence
from absl import app
from absl import flags

import genc as genc

ADDRESS = flags.DEFINE_string(
    "server_address", None, "The address of the server.", required=True
)
DIGEST = flags.DEFINE_string(
    "image_digest", None, "The image digest.", required=True
)


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError("Too many command-line arguments.")

  genc.runtime.set_default_executor()

  # Computation to be executed in a secure enclave on Confidential Computing.
  @genc.authoring.traced_computation
  def foo(x):
    prompt_template = genc.authoring.prompt_template["Tell me about {topic}"]
    model_inference = genc.authoring.model_inference_with_config[{
        "model_uri": "/device/gemma",
        "model_config": {"model_path": "/gemma-2b-it-q4_k_m.gguf"}}]
    return model_inference(prompt_template(x))

  # Computation to run locally, including the delegation of `foo` to the TEE.
  @genc.authoring.traced_computation
  def bar(x):
    backend = {"server_address": ADDRESS.value, "image_digest": DIGEST.value}
    return genc.authoring.confidential_computation[foo, backend](x)

  result = bar("scuba diving")
  print(result)


if __name__ == "__main__":
  app.run(main)
