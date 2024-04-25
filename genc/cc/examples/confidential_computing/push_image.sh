#!/bin/bash
#
# Copyright 2023, The GenC Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# ou may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License
# ==============================================================================
#
# Pushes a worker image to GCP.

source config_environment.sh

docker push \
  us-docker.pkg.dev/${WORKER_PROJECT}/${WORKER_REPO}/${WORKER_IMAGE}:latest
