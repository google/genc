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
"""Libraries fo constructing runners for executing generative computations."""

from generative_computing.cc.runtime import executor_bindings
from generative_computing.proto.v0 import computation_pb2 as pb


class Runner(object):
  """Represents a runner."""

  def __init__(self, comp_pb: pb.Computation):
    """Construct a runner for the given computation proto.

    Args:
      comp_pb: The computation proto to contruct the runner for.
    """
    self._executor = executor_bindings.create_default_local_executor()
    self._comp_pb = comp_pb

  def __call__(self, *args, **kwargs):
    """Invoke the runner on a given set of arguments.

    This is a blocking call that performs the computation synchronously.

    Args:
      *args: Positonal args.
      **kwargs: Keyword args.

    Returns:
      The result of the computation, computed synchronously.
    """
    # TODO(b/295042075): Implement this.
    return None
