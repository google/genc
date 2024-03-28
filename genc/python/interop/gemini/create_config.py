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
"""Utility functions used in Gemini interop."""

# Gemini-Pro endpoint for REST calls from Google AI Studio.
# URL reference: https://ai.google.dev/tutorials/rest_quickstart
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


def create_config(
    api_key,
    endpoint=GOOGLE_AI_STUDIO_GEMINI_PRO_ENDPOINT,
    json_request_template=GEMINI_PRO_TEXT_GENERATION_JSON_TEMPLATE,
):
  """Creates a dictionary containing model inference configuration used in creating and sending requests to Gemini.

  Args:
    api_key: An API key used to access the model.
    endpoint: A gemini model serving endpoint.
    json_request_template: A fully specified JSON request with query contents,
      safety settings, generation config. The text field of the last part of the
      last content in the template is replaced by dynamic prompt.

  Returns:
    An instance of the `dict` object.
  """
  config = {
      "endpoint": endpoint,
      "api_key": api_key,
      "json_request_template": json_request_template,
  }
  return config
