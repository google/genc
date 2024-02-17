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
"""Test for create_config.py."""

from absl.testing import absltest

from generative_computing.python.interop.gemini import create_config


API_KEY = "test_key"
ENDPOINT = "test_endpoint"
JSON_REQUEST_TEMPLATE = "test_template"
GOOGLE_AI_STUDIO_GEMINI_PRO_ENDPOINT = "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent"
GEMINI_PRO_TEXT_GENERATION_JSON_TEMPLATE = """
{"contents":[{"role":"user","parts":[{"text":"PROMPT REPLACES THIS"}]}],
 "safety_settings":[{
    "category":"HARM_CATEGORY_SEXUALLY_EXPLICIT",
    "threshold":"BLOCK_LOW_AND_ABOVE"}],
  "generation_config":{
    "temperature":0.9,
    "topP":1.0,
    "maxOutputTokens":1024}
}
"""


class CreateConfigTest(absltest.TestCase):

  def test_create_config(self):
    expected_config_1 = {
        "api_key": API_KEY,
        "endpoint": GOOGLE_AI_STUDIO_GEMINI_PRO_ENDPOINT,
        "json_request_template": GEMINI_PRO_TEXT_GENERATION_JSON_TEMPLATE,
    }
    expected_config_2 = {
        "api_key": API_KEY,
        "endpoint": ENDPOINT,
        "json_request_template": GEMINI_PRO_TEXT_GENERATION_JSON_TEMPLATE,
    }
    expected_config_3 = {
        "api_key": API_KEY,
        "endpoint": ENDPOINT,
        "json_request_template": JSON_REQUEST_TEMPLATE,
    }

    actual_config_1 = create_config.create_config(api_key=API_KEY)
    actual_config_2 = create_config.create_config(
        api_key=API_KEY, endpoint=ENDPOINT
    )
    actual_config_3 = create_config.create_config(
        api_key=API_KEY,
        endpoint=ENDPOINT,
        json_request_template=JSON_REQUEST_TEMPLATE,
    )
    self.assertEqual(actual_config_1, expected_config_1)
    self.assertEqual(actual_config_2, expected_config_2)
    self.assertEqual(actual_config_3, expected_config_3)


if __name__ == "__main__":
  absltest.main()
