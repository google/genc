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
"""Tests for tracing classes."""

from absl.testing import absltest
from generative_computing.python import runtime
from generative_computing.python.authoring import constructors
from generative_computing.python.authoring import tracing_decorator
from generative_computing.python.authoring import tracing_intrinsics


class TracingTest(absltest.TestCase):

  def test_something(self):

    @tracing_decorator.traced_computation
    def foo(x, y):
      del x
      return y

    @tracing_decorator.traced_computation
    def bar(x, y, z):
      return foo(foo(x, y), z)

    bar_runner = runtime.Runner(bar.portable_ir)
    result = bar_runner('a', 'b', 'c')
    self.assertEqual(result, 'c')

  def test_something_else(self):
    comp = tracing_intrinsics.model_inference['/device/ulm1b']
    self.assertEqual(
        str(comp.portable_ir), str(constructors.create_model('/device/ulm1b'))
    )


if __name__ == '__main__':
  absltest.main()
