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
FROM ubuntu:latest

ENV LANG C.UTF-8

RUN apt-get update && apt-get install -y \
    python3.10 \
    python3.10-dev \
    python3-pip \
    git

COPY requirements.txt /requirements.txt

RUN pip3 install --require-hashes -r requirements.txt

RUN ln -s -f $(which ${PYTHON}) /usr/local/bin/python
RUN python3 --version

RUN apt update && apt install -y \
    apt-transport-https \
    bash \
    build-essential \
    curl \
    g++ \
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

RUN apt-get update && \
    apt-get install -y --no-install-recommends default-jdk

# Install deps for running a GenC Jupyter Notebook (used in Tutorials)
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt install -y jupyter-notebook

RUN apt-get install jupyter-notebook
