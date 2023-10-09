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
"""Test for constructors.py."""

from absl.testing import absltest
from generative_computing.python.authoring import constructors


class ConstructorsTest(absltest.TestCase):

  def test_prompt_template(self):
    comp_pb = constructors.create_prompt_template('foo')
    self.assertEqual(comp_pb.WhichOneof('computation'), 'intrinsic')
    self.assertEqual(comp_pb.intrinsic.uri, 'prompt_template')
    self.assertLen(comp_pb.intrinsic.static_parameter, 1)
    self.assertEqual(
        comp_pb.intrinsic.static_parameter[0].name, 'template_string')
    self.assertEqual(comp_pb.intrinsic.static_parameter[0].value.str, 'foo')

  def test_model_inference(self):
    comp_pb = constructors.create_model('foo')
    self.assertEqual(comp_pb.WhichOneof('computation'), 'intrinsic')
    self.assertEqual(comp_pb.intrinsic.uri, 'model_inference')
    self.assertLen(comp_pb.intrinsic.static_parameter, 1)
    self.assertEqual(comp_pb.intrinsic.static_parameter[0].name, 'model_uri')
    self.assertEqual(comp_pb.intrinsic.static_parameter[0].value.str, 'foo')

  def test_fallback(self):
    comp_pb = constructors.create_fallback([
        constructors.create_model('a'), constructors.create_model('b')])
    self.assertEqual(comp_pb.WhichOneof('computation'), 'intrinsic')
    self.assertEqual(comp_pb.intrinsic.uri, 'fallback')
    self.assertLen(comp_pb.intrinsic.static_parameter, 2)
    for idx, model_name in enumerate(['a', 'b']):
      self.assertEqual(
          comp_pb.intrinsic.static_parameter[idx].name, 'candidate_fn')
      self.assertEqual(
          str(comp_pb.intrinsic.static_parameter[idx].value.computation),
          str(constructors.create_model(model_name)))

  def test_conditional(self):
    condition = constructors.create_reference('some_bool_condition')
    comp_pb = constructors.create_conditional(
        condition,
        constructors.create_model('a'),
        constructors.create_model('b'),
    )
    self.assertEqual(comp_pb.WhichOneof('computation'), 'call')
    self.assertEqual(str(comp_pb.call.argument), str(condition))
    conditional_pb = comp_pb.call.function
    self.assertEqual(conditional_pb.WhichOneof('computation'), 'intrinsic')
    self.assertEqual(conditional_pb.intrinsic.uri, 'conditional')
    self.assertLen(conditional_pb.intrinsic.static_parameter, 2)
    for idx, param_name, model_name in [(0, 'then', 'a'), (1, 'else', 'b')]:
      self.assertEqual(
          conditional_pb.intrinsic.static_parameter[idx].name, param_name
      )
      self.assertEqual(
          str(conditional_pb.intrinsic.static_parameter[idx].value.computation),
          str(constructors.create_model(model_name)),
      )

  def test_regex_partial_match(self):
    comp_pb = constructors.create_regex_partial_match('A: True')
    self.assertEqual(comp_pb.WhichOneof('computation'), 'intrinsic')
    self.assertEqual(comp_pb.intrinsic.uri, 'regex_partial_match')
    self.assertLen(comp_pb.intrinsic.static_parameter, 1)
    self.assertEqual(
        comp_pb.intrinsic.static_parameter[0].name, 'pattern_string')
    self.assertEqual(comp_pb.intrinsic.static_parameter[0].value.str, 'A: True')

if __name__ == '__main__':
  absltest.main()
