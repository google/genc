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
    self.assertEqual(comp_pb.WhichOneof('value'), 'intrinsic')
    self.assertEqual(comp_pb.intrinsic.uri, 'prompt_template')
    self.assertEqual(comp_pb.intrinsic.static_parameter.str, 'foo')

  def test_model_inference(self):
    comp_pb = constructors.create_model('foo')
    self.assertEqual(comp_pb.WhichOneof('value'), 'intrinsic')
    self.assertEqual(comp_pb.intrinsic.uri, 'model_inference')
    self.assertEqual(comp_pb.intrinsic.static_parameter.str, 'foo')

  def test_fallback(self):
    comp_pb = constructors.create_fallback(
        [constructors.create_model('a'), constructors.create_model('b')]
    )
    self.assertEqual(comp_pb.WhichOneof('value'), 'intrinsic')
    self.assertEqual(comp_pb.intrinsic.uri, 'fallback')
    self.assertLen(comp_pb.intrinsic.static_parameter.struct.element, 2)
    for idx, model_name in enumerate(['a', 'b']):
      self.assertEqual(
          comp_pb.intrinsic.static_parameter.struct.element[idx].name,
          'candidate_fn',
      )
      self.assertEqual(
          str(comp_pb.intrinsic.static_parameter.struct.element[idx].value),
          str(constructors.create_model(model_name)),
      )

  def test_conditional(self):
    condition = constructors.create_reference('some_bool_condition')
    comp_pb = constructors.create_conditional(
        condition,
        constructors.create_model('a'),
        constructors.create_model('b'),
    )
    self.assertEqual(comp_pb.WhichOneof('value'), 'call')
    self.assertEqual(str(comp_pb.call.argument), str(condition))
    conditional_pb = comp_pb.call.function
    self.assertEqual(conditional_pb.WhichOneof('value'), 'intrinsic')
    self.assertEqual(conditional_pb.intrinsic.uri, 'conditional')
    self.assertLen(conditional_pb.intrinsic.static_parameter.struct.element, 2)
    for idx, param_name, model_name in [(0, 'then', 'a'), (1, 'else', 'b')]:
      self.assertEqual(
          conditional_pb.intrinsic.static_parameter.struct.element[idx].name,
          param_name,
      )
      self.assertEqual(
          str(
              conditional_pb.intrinsic.static_parameter.struct.element[
                  idx
              ].value
          ),
          str(constructors.create_model(model_name)),
      )

  def test_repeat(self):
    comp_pb = constructors.create_repeat(num_steps=5, body_fn='body_fn_name')

    static_params = comp_pb.intrinsic.static_parameter.struct.element
    self.assertEqual(static_params[0].value.str, '5')
    self.assertEqual(static_params[1].value.str, 'body_fn_name')

  def test_regex_partial_match(self):
    comp_pb = constructors.create_regex_partial_match('A: True')
    self.assertEqual(comp_pb.WhichOneof('value'), 'intrinsic')
    self.assertEqual(comp_pb.intrinsic.uri, 'regex_partial_match')
    self.assertEqual(comp_pb.intrinsic.static_parameter.str, 'A: True')

  def test_while(self):
    test_condition_fn = constructors.create_regex_partial_match(
        'stop_keyword: Finish'
    )
    test_body_fn = constructors.create_model('test_model')
    while_pb = constructors.create_while(test_condition_fn, test_body_fn)
    self.assertEqual(while_pb.WhichOneof('value'), 'intrinsic')
    self.assertEqual(while_pb.intrinsic.uri, 'while')
    self.assertLen(while_pb.intrinsic.static_parameter.struct.element, 2)

    for idx, param_name, fn in [
        (0, 'condition_fn', str(test_condition_fn)),
        (1, 'body_fn', str(test_body_fn)),
    ]:
      self.assertEqual(
          while_pb.intrinsic.static_parameter.struct.element[idx].name,
          param_name,
      )
      self.assertEqual(
          str(while_pb.intrinsic.static_parameter.struct.element[idx].value),
          fn,
      )


if __name__ == '__main__':
  absltest.main()