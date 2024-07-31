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
from genc.cc.runtime import executor_bindings
from genc.python import authoring
from genc.python import runtime
from genc.python.runtime import runner


class RunnerTest(absltest.TestCase):

  def test_model(self):
    executor = executor_bindings.create_default_local_executor()
    comp_pb = authoring.create_model('test_model')
    comp = runner.Runner(comp_pb, executor)
    result = comp('Boo!')
    self.assertEqual(
        result, 'This is an output from a test model in response to "Boo!".'
    )

  def test_prompt_template(self):
    executor = executor_bindings.create_default_local_executor()
    comp_pb = authoring.create_prompt_template(
        'Q: What should I pack for a trip to {location}? A: '
    )
    comp = runner.Runner(comp_pb, executor)
    result = comp('a grocery store')
    self.assertEqual(
        result, 'Q: What should I pack for a trip to a grocery store? A: '
    )

  def test_chain(self):
    executor = executor_bindings.create_default_local_executor()
    comp_pb = authoring.create_serial_chain([
        authoring.create_prompt_template(
            'Q: What should I pack for a trip to {location}? A: '
        ),
        authoring.create_model('test_model'),
    ])
    comp = runner.Runner(comp_pb, executor)
    result = comp('a grocery store')
    self.assertEqual(
        result,
        'This is an output from a test model in response to "Q: What should I'
        ' pack for a trip to a grocery store? A: ".',
    )

  def test_fallback(self):
    executor = executor_bindings.create_default_local_executor()
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
      comp = runner.Runner(comp_pb, executor)
      if expected_result:
        result = comp(arg)
        self.assertEqual(result, expected_result)
      else:
        with self.assertRaises(RuntimeError):
          comp(arg)

  def test_conditional_with_valid_inputs(self):
    executor = executor_bindings.create_default_local_executor()
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
    comp = runner.Runner(comp_pb, executor)
    result1 = comp(True, 'kiki', 'bubba')
    self.assertEqual(
        result1, 'This is an output from a test model in response to "kiki".'
    )
    result2 = comp(False, 'kiki', 'bubba')
    self.assertEqual(
        result2, 'This is an output from a test model in response to "bubba".'
    )

  def test_conditional_is_lazy(self):
    executor = executor_bindings.create_default_local_executor()
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
    comp = runner.Runner(comp_pb, executor)
    result1 = comp(True, 'kiki')
    self.assertEqual(
        result1, 'This is an output from a test model in response to "kiki".'
    )
    with self.assertRaises(RuntimeError):
      comp(False, 'kiki')

  def test_regex_partial_match(self):
    executor = executor_bindings.create_default_local_executor()
    comp_pb = authoring.create_regex_partial_match('A: True|true')
    comp = runner.Runner(comp_pb, executor)
    result = comp('A: True. Explanation for true.')
    self.assertTrue(result)
    result2 = comp('The statement appears to be true.')
    self.assertTrue(result2)
    result3 = comp('A: False. Explanation for false.')
    self.assertFalse(result3)

  def test_partial_match_with_conditional(self):
    executor = executor_bindings.create_default_local_executor()
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
    comp = runner.Runner(comp_pb, executor)
    result1 = comp('True', 'kiki', 'bubba')
    self.assertEqual(
        result1, 'This is an output from a test model in response to "kiki".'
    )
    result2 = comp('False', 'kiki', 'bubba')
    self.assertEqual(
        result2, 'This is an output from a test model in response to "bubba".'
    )

  def test_multivariate_prompt_template(self):
    executor = executor_bindings.create_default_local_executor()
    comp_pb = authoring.create_prompt_template_with_parameters(
        'A template in which a foo is {foo} and a bar is {bar}.',
        ['foo', 'bar'])
    comp = runner.Runner(comp_pb, executor)
    result = comp('XXX', 'YYY')
    self.assertEqual(
        str(result), 'A template in which a foo is XXX and a bar is YYY.')

  def test_more_fancy_multivariate_prompt_template(self):
    executor = executor_bindings.create_default_local_executor()
    comp_pb = authoring.create_prompt_template_with_parameters(
        'First {foo} then {bar} then some more {bar} and {foo} and {foo}.',
        ['foo', 'bar'])
    comp = runner.Runner(comp_pb, executor)
    result = comp('XXX', 'YYY')
    self.assertEqual(
        str(result),
        'First XXX then YYY then some more YYY and XXX and XXX.')

  def test_multivariate_prompt_template_with_tracing_version_1(self):
    @authoring.traced_computation
    def comp(arg):
      return authoring.prompt_template_with_parameters[
          'First {foo} then {bar} then some more {bar} and {foo} and {foo}.',
          ['foo', 'bar']](arg)

    executor = executor_bindings.create_default_local_executor()
    runtime.set_default_executor(executor)
    result = comp('XXX', 'YYY')
    self.assertEqual(
        str(result),
        'First XXX then YYY then some more YYY and XXX and XXX.')

  def test_multivariate_prompt_template_with_tracing_version_2(self):
    @authoring.traced_computation
    def comp(arg):
      prompt_template = authoring.create_prompt_template_with_parameters(
          'First {foo} then {bar} then some more {bar} and {foo} and {foo}.',
          ['foo', 'bar'])
      return authoring.create_call(prompt_template, arg)

    executor = executor_bindings.create_default_local_executor()
    runtime.set_default_executor(executor)
    result = comp('XXX', 'YYY')
    self.assertEqual(
        str(result),
        'First XXX then YYY then some more YYY and XXX and XXX.')

  def test_chained_multivariate_prompt_templates(self):
    @authoring.traced_computation
    def comp(aspect, verb, noun):
      template_1 = authoring.prompt_template_with_parameters[
          'Tell me more about the {aspect} aspect of {activity}.',
          ['aspect', 'activity']]
      template_2 = authoring.prompt_template_with_parameters[
          '{noun} {verb}',
          ['noun', 'verb']]
      return template_1(aspect, template_2(noun, verb))

    executor = executor_bindings.create_default_local_executor()
    runtime.set_default_executor(executor)
    result = comp('financial', 'diving', 'scuba')
    self.assertEqual(
        str(result),
        'Tell me more about the financial aspect of scuba diving.')

  def test_two_multivariate_templates_interleaved_with_model_calls(self):
    @authoring.traced_computation
    def comp(aspect, activity, additional_aspect):
      template_1 = authoring.prompt_template_with_parameters[
          'Tell me about the {aspect} aspect of {activity}.',
          ['aspect', 'activity']]
      model_1 = authoring.model_inference['test_model']
      first_result = model_1(template_1(aspect, activity))
      template_2 = authoring.prompt_template_with_parameters[
          'Expand on the following passage by focusing on the '  # pylint: disable=implicit-str-concat
          'additional aspect {additional_aspect}: "{passage}.',
          ['passage', 'additional_aspect']]
      second_result = model_1(template_2(first_result, additional_aspect))
      return second_result

    executor = executor_bindings.create_default_local_executor()
    runtime.set_default_executor(executor)
    result = comp('financial', 'scuba diving', 'fun')
    self.assertEqual(
        str(result),
        'This is an output from a test model in response to "Expand on the '
        'following passage by focusing on the additional aspect fun: '
        '"This is an output from a test model in response to "Tell me about '
        'the financial aspect of scuba diving."..".')


if __name__ == '__main__':
  absltest.main()
