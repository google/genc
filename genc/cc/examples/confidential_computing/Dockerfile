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
FROM ubuntu:22.04

ENV LANG C.UTF-8

LABEL "tee.launch_policy.log_redirect"="always"

RUN apt update && apt install -y \
    apt-transport-https \
    bash \
    build-essential \
    curl \
    g++ \
    git \
    gnupg \
    openjdk-8-jdk \
    pkg-config \
    swig \
    unzip \
    wget \
    zlib1g-dev \
    zip

RUN curl -fsSL https://bazel.build/bazel-release.pub.gpg \
    | gpg --dearmor >bazel-archive-keyring.gpg

RUN mv bazel-archive-keyring.gpg /usr/share/keyrings

RUN echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list

RUN apt update && apt install -y bazel-6.3.2

RUN ln -s -f /usr/bin/bazel-6.3.2 /usr/bin/bazel

WORKDIR "/"

RUN git clone https://github.com/google/genc

RUN wget --directory-prefix=/ https://huggingface.co/lmstudio-ai/gemma-2b-it-GGUF/resolve/main/gemma-2b-it-q4_k_m.gguf

EXPOSE 80/tcp

WORKDIR "/genc"

RUN bazel build genc/cc/examples/worker:server

CMD bazel-bin/genc/cc/examples/worker/server --port=80 --oak=true --debug=true
