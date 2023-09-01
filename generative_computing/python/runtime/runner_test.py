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
"""Test for runner.py."""

from absl.testing import absltest
from generative_computing.proto.v0 import computation_pb2 as pb
from generative_computing.python.runtime import runner


class RunnerTest(absltest.TestCase):

  def test_model(self):
    comp_pb = pb.Computation(
        model=pb.Model(model_id=pb.ModelId(uri='test_model'))
    )
    comp = runner.Runner(comp_pb)
    result = comp('Boo!')
    self.assertEqual(
        result, 'This is an output from a test model in response to "Boo!".'
    )


if __name__ == '__main__':
  absltest.main()
