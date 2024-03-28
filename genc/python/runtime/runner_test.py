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
"""Test for runner.py."""

from absl.testing import absltest
from genc.python import authoring
from genc.python.runtime import runner


class RunnerTest(absltest.TestCase):

  def test_model(self):
    comp_pb = authoring.create_model('test_model')
    comp = runner.Runner(comp_pb)
    result = comp('Boo!')
    self.assertEqual(
        result, 'This is an output from a test model in response to "Boo!".'
    )

  def test_prompt_template(self):
    comp_pb = authoring.create_prompt_template(
        'Q: What should I pack for a trip to {location}? A: '
    )
    comp = runner.Runner(comp_pb)
    result = comp('a grocery store')
    self.assertEqual(
        result, 'Q: What should I pack for a trip to a grocery store? A: '
    )

  def test_chain(self):
    comp_pb = authoring.create_serial_chain([
        authoring.create_prompt_template(
            'Q: What should I pack for a trip to {location}? A: '
        ),
        authoring.create_model('test_model'),
    ])
    comp = runner.Runner(comp_pb)
    result = comp('a grocery store')
    self.assertEqual(
        result,
        'This is an output from a test model in response to "Q: What should I'
        ' pack for a trip to a grocery store? A: ".',
    )

  def test_fallback(self):
    arg = 'a grocery store'
    good_model = authoring.create_model('test_model')
    good_model_output = (
        'This is an output from a test model in response to "{}".'.format(arg)
    )
    bad_model = authoring.create_model('ftp://nonexistent.model:000000')
    good_chain = authoring.create_serial_chain([
        authoring.create_prompt_template('Q: Tell me about {topic}. A: '),
        authoring.create_model('test_model'),
    ])
    good_chain_output = (
        'This is an output from a test model in response to "Q: Tell me about'
        ' {}. A: ".'.format(arg)
    )
    test_cases = [
        (authoring.create_fallback([bad_model, good_model]), good_model_output),
        (authoring.create_fallback([]), None),
        (authoring.create_fallback([bad_model]), None),
        (authoring.create_fallback([bad_model, bad_model]), None),
        (authoring.create_fallback([good_model]), good_model_output),
        (authoring.create_fallback([good_chain]), good_chain_output),
        (authoring.create_fallback([good_model, bad_model]), good_model_output),
        (
            authoring.create_fallback([good_model, good_chain]),
            good_model_output,
        ),
        (
            authoring.create_fallback([good_chain, good_model]),
            good_chain_output,
        ),
        (authoring.create_fallback([good_chain, bad_model]), good_chain_output),
        (authoring.create_fallback([bad_model, good_chain]), good_chain_output),
    ]
    for comp_pb, expected_result in test_cases:
      comp = runner.Runner(comp_pb)
      if expected_result:
        result = comp(arg)
        self.assertEqual(result, expected_result)
      else:
        with self.assertRaises(RuntimeError):
          comp(arg)

  def test_conditional_with_valid_inputs(self):
    arg = authoring.create_reference('x')
    model = authoring.create_model('test_model')
    comp_pb = authoring.create_lambda(
        arg.reference.name,
        authoring.create_conditional(
            authoring.create_selection(arg, 0),
            authoring.create_call(model, authoring.create_selection(arg, 1)),
            authoring.create_call(model, authoring.create_selection(arg, 2)),
        ),
    )
    comp = runner.Runner(comp_pb)
    result1 = comp(True, 'kiki', 'bubba')
    self.assertEqual(
        result1, 'This is an output from a test model in response to "kiki".'
    )
    result2 = comp(False, 'kiki', 'bubba')
    self.assertEqual(
        result2, 'This is an output from a test model in response to "bubba".'
    )

  def test_conditional_is_lazy(self):
    arg = authoring.create_reference('x')
    comp_pb = authoring.create_lambda(
        arg.reference.name,
        authoring.create_conditional(
            authoring.create_selection(arg, 0),
            authoring.create_call(
                authoring.create_model('test_model'),
                authoring.create_selection(arg, 1),
            ),
            authoring.create_reference('broken_undefined_variable_will_fail'),
        ),
    )
    comp = runner.Runner(comp_pb)
    result1 = comp(True, 'kiki')
    self.assertEqual(
        result1, 'This is an output from a test model in response to "kiki".'
    )
    with self.assertRaises(RuntimeError):
      comp(False, 'kiki')

  def test_regex_partial_match(self):
    comp_pb = authoring.create_regex_partial_match('A: True|true')
    comp = runner.Runner(comp_pb)
    result = comp('A: True. Explanation for true.')
    self.assertTrue(result)
    result2 = comp('The statement appears to be true.')
    self.assertTrue(result2)
    result3 = comp('A: False. Explanation for false.')
    self.assertFalse(result3)

  def test_partial_match_with_conditional(self):
    arg = authoring.create_reference('x')
    scorer_chain = authoring.create_serial_chain([
        authoring.create_prompt_template(
            'Q: Is following sentence political or sensitive? Who is going'
            ' to be the next president? A: {answer}'
        ),  # supply true or false in answer
        authoring.create_model('test_model'),
        authoring.create_regex_partial_match('A: True|A: true|true|True'),
    ])
    model = authoring.create_model('test_model')
    comp_pb = authoring.create_lambda(
        arg.reference.name,
        authoring.create_conditional(
            authoring.create_call(
                scorer_chain, authoring.create_selection(arg, 0)
            ),
            authoring.create_call(model, authoring.create_selection(arg, 1)),
            authoring.create_call(model, authoring.create_selection(arg, 2)),
        ),
    )
    comp = runner.Runner(comp_pb)
    result1 = comp('True', 'kiki', 'bubba')
    self.assertEqual(
        result1, 'This is an output from a test model in response to "kiki".'
    )
    result2 = comp('False', 'kiki', 'bubba')
    self.assertEqual(
        result2, 'This is an output from a test model in response to "bubba".'
    )


if __name__ == '__main__':
  absltest.main()
