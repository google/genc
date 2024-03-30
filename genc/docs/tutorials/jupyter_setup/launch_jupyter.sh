#!/bin/bash


# /* Copyright 2023, The GenC Authors.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#      http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License
# ==============================================================================*/

#install jupyter notebook and pip requirements

apt install jupyter-notebook

pip install -r docs/tutorials/jupyter_setup/pip_jupyter.txt

bazel run docs/tutorials/jupyter_setup:jupyter