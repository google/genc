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
"""Tests for computation.py."""

from absl.testing import absltest
from generative_computing.proto.v0 import computation_pb2 as pb
from generative_computing.python.base import computation
from generative_computing.python.base import context


class TestContext(context.Context):

  def call(self, call_target, *args, **kwargs):
    return 'called({})'.format(str(call_target).strip())


class ComputationTest(absltest.TestCase):

  def test_something(self):
    ctx = TestContext()
    context.context_stack.append_nested_context(ctx)
    comp = computation.Computation(pb.Value(str='foo'))
    result = comp('dupa')
    context.context_stack.remove_nested_context(ctx)
    self.assertEqual(result, 'called(str: "foo")')


if __name__ == '__main__':
  absltest.main()
