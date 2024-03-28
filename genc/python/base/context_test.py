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
"""Tests for context.py."""

from absl.testing import absltest
from genc.python.base import context


class TestContext(context.Context):

  def call(self, call_target, *args, **kwargs):
    pass


class ContextTest(absltest.TestCase):

  def test_something(self):
    stk = context.ContextStack()
    self.assertIsNone(stk.current_context)
    c1 = TestContext()
    stk.append_nested_context(c1)
    self.assertIs(stk.current_context, c1)
    c2 = TestContext()
    stk.append_nested_context(c2)
    self.assertIs(stk.current_context, c2)
    with self.assertRaises(RuntimeError):
      stk.remove_nested_context(c1)
    c3 = TestContext()
    stk.set_default_context(c3)
    self.assertIs(stk.current_context, c2)
    stk.remove_nested_context(c2)
    self.assertIs(stk.current_context, c1)
    stk.remove_nested_context(c1)
    self.assertIs(stk.current_context, c3)


if __name__ == '__main__':
  absltest.main()
