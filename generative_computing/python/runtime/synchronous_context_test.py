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
"""Test for synchronous_context.py."""

from absl.testing import absltest
from generative_computing.python.authoring import tracing_decorator
from generative_computing.python.runtime import synchronous_context


class TracingTest(absltest.TestCase):

  def test_something(self):
    synchronous_context.set_default_executor()

    @tracing_decorator.traced_computation
    def foo(x, y):
      del x
      return y

    @tracing_decorator.traced_computation
    def bar(x, y, z):
      return foo(foo(x, y), z)

    self.assertEqual(bar('a', 'b', 'c'), 'c')


if __name__ == '__main__':
  absltest.main()
