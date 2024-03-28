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
"""Python wrappers for authoring computations."""

from genc.python.authoring.constructors import create_breakable_chain
from genc.python.authoring.constructors import create_call
from genc.python.authoring.constructors import create_conditional
from genc.python.authoring.constructors import create_custom_function
from genc.python.authoring.constructors import create_fallback
from genc.python.authoring.constructors import create_inja_template
from genc.python.authoring.constructors import create_lambda
from genc.python.authoring.constructors import create_lambda_from_fn
from genc.python.authoring.constructors import create_logger
from genc.python.authoring.constructors import create_logical_not
from genc.python.authoring.constructors import create_model
from genc.python.authoring.constructors import create_model_with_config
from genc.python.authoring.constructors import create_named_value
from genc.python.authoring.constructors import create_parallel_map
from genc.python.authoring.constructors import create_prompt_template
from genc.python.authoring.constructors import create_reference
from genc.python.authoring.constructors import create_regex_partial_match
from genc.python.authoring.constructors import create_repeat
from genc.python.authoring.constructors import create_repeated_conditional_chain
from genc.python.authoring.constructors import create_rest_call
from genc.python.authoring.constructors import create_rest_model_config
from genc.python.authoring.constructors import create_selection
from genc.python.authoring.constructors import create_serial_chain
from genc.python.authoring.constructors import create_struct
from genc.python.authoring.constructors import create_while
from genc.python.authoring.constructors import create_wolfram_alpha
from genc.python.authoring.tracing_decorator import traced_computation
from genc.python.authoring.tracing_intrinsics import *
