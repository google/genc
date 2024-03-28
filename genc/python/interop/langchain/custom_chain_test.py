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
"""Test for custom_chain.py."""

from absl.testing import absltest
from langchain.chains import base
from genc.python.interop.langchain import custom_chain


class CustomModelTest(absltest.TestCase):

  def test_creation(self):
    my_chain = custom_chain.CustomChain(chained_ops=[])
    self.assertIsInstance(my_chain, base.Chain)
    self.assertEqual(my_chain.chained_ops, [])
    self.assertEqual(my_chain._chain_type, 'genc_custom_chain')


if __name__ == '__main__':
  absltest.main()
