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
# Creates a debug VM on GCP with a Confidential Space image running a worker.

source config_environment.sh

gcloud compute instances create ${WORKER_VM} \
  --confidential-compute \
  --shielded-secure-boot \
  --scopes=cloud-platform \
  --zone=${WORKER_ZONE} \
  --image-project=confidential-space-images \
  --image-family=confidential-space-debug \
  --metadata="^~^tee-image-reference=us-docker.pkg.dev/${WORKER_PROJECT}/${WORKER_REPO}/${WORKER_IMAGE}:latest~tee-container-log-redirect=true" \
  --machine-type="n2d-standard-4" \
  --min-cpu-platform="AMD Milan" \
  --tags http-server,https-server \
  --service-account=${WORKER_ACCOUNT}@${WORKER_PROJECT}.iam.gserviceaccount.com
