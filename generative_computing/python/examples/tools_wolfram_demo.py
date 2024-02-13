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
"""A simple example that shows how to execute execute tools."""

from collections.abc import Sequence

from absl import app
from absl import flags

from generative_computing.proto.v0 import computation_pb2 as pb
from generative_computing.python import authoring
from generative_computing.python import runtime

# An example that calls WolframAlpha as part of the computation.
FLAGS = flags.FLAGS
flags.DEFINE_string("appid", None, "appid of WolframAlpha", required=True)


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError("Too many command-line arguments.")

  tool_call = authoring.create_wolfram_alpha(FLAGS.appid)
  comp = runtime.Runner(comp_pb=tool_call)

  request = pb.Value()
  request.str = "what is the result of 2^2-2-3+4*100"
  print(comp(request))


if __name__ == "__main__":
  app.run(main)
