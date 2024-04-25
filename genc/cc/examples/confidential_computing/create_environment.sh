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
# Creates the basic environment (accounts, permissions, repositories, etc.).

source config_environment.sh

gcloud config set project ${WORKER_PROJECT}

gcloud iam service-accounts create ${WORKER_ACCOUNT}

gcloud projects add-iam-policy-binding ${WORKER_PROJECT} \
  --member="serviceAccount:${WORKER_ACCOUNT}@g${WORKER_PROJECT}.iam.gserviceaccount.com" \
  --role="roles/confidentialcomputing.workloadUser"

gcloud projects add-iam-policy-binding ${WORKER_PROJECT} \
  --member="serviceAccount:${WORKER_ACCOUNT}@g${WORKER_PROJECT}.iam.gserviceaccount.com" \
  --role="roles/logging.logWriter"

gcloud projects add-iam-policy-binding ${WORKER_PROJECT} \
  --member="serviceAccount:${WORKER_ACCOUNT}@g${WORKER_PROJECT}.iam.gserviceaccount.com" \
  --role="roles/artifactregistry.reader"

gcloud artifacts repositories create ${WORKER_REPO} \
  --repository-format=docker \
  --location=us

gcloud auth configure-docker us-docker.pkg.dev
