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
"""Tests for embed.py."""

from absl.testing import absltest
from genc.proto.v0 import computation_pb2 as pb
from genc.python.authoring import constructors
from genc.python.authoring import tracing_decorator
from genc.python.base.embed import embed as embed_func


class EmbedTest(absltest.TestCase):

  def test_something(self):

    foo = constructors.create_custom_function('foo')

    @tracing_decorator.traced_computation
    def bar(x):
      my_foo = embed_func(foo)
      return my_foo(my_foo(x))

    expected_bar = constructors.create_lambda(
        'arg',
        pb.Value(
            block=pb.Block(
                local=[
                    pb.Block.Local(
                        name='v_0',
                        value=constructors.create_custom_function('foo'),
                    ),
                    pb.Block.Local(
                        name='v_1',
                        value=constructors.create_call(
                            constructors.create_reference('v_0'),
                            constructors.create_reference('arg'),
                        ),
                    ),
                    pb.Block.Local(
                        name='v_2',
                        value=constructors.create_call(
                            constructors.create_reference('v_0'),
                            constructors.create_reference('v_1'),
                        ),
                    ),
                ],
                result=constructors.create_reference('v_2'),
            )
        ),
    )

    self.assertEqual(str(bar.portable_ir), str(expected_bar))


if __name__ == '__main__':
  absltest.main()
