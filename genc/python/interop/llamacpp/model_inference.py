# Copyright 2024, The GenC Authors.
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
"""Utility functions used in LlamaCpp interop."""

from genc.proto.v0 import computation_pb2 as pb
from genc.python import base
from genc.python.interop.llamacpp.create_config import create_config


def model_inference(model_uri, model_path, **kwargs):
  """Creates a model_inference_with_config operator setup for use with LlamaCpp.

  Args:
    model_uri: The URI of the model (backed by a LlamaCpp handler).
    model_path: The path to the model parameters.
    **kwargs: Optional keyword arguments to pass to LlamaCpp, such as the
      number of threads or max number of tokens.

  Returns:
    Same as `model_inference_with_config` but with the model URI, model path,
    and optional LLamaCpp parameters added to the config.
  """
  return base.Computation(pb.Value(intrinsic=pb.Intrinsic(
      uri="model_inference_with_config",
      static_parameter=base.to_value_proto({
          "model_uri": model_uri,
          "model_config": create_config(model_path, **kwargs)
      }))))
