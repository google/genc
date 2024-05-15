# Setup GenC

A quick and easy way to setup environment for GenC is using Docker Container:

### 1. Clone git repository

```
git clone https://github.com/google/genc
```

### 2.Build Docker Container

```
cd genc
docker build -t genc .
docker run -it -v $PWD:/genc genc:latest /bin/bash -c "cd genc; /bin/bash"
```

Check if you have successfully built docker container. Check the active images
by running "docker images".

### 3. Test and run examples

Inside your running docker container instance, at the root of the repo, run:

```
bazel test genc/...
```

You can navigate to python/examples and pick your favorite example and run

```
bazel run python/examples:$favorite_example
```

## Access GenC tutorials with a Jupyter notebook

Building a Jupyter notebook in docker container involves some extra setup, but
it could be useful when you want to quickly test few things, and it will enable
you to run all the included tutorials.

### 1. Add open port to docker.

Exit the docker container, and restart it with an open port 8889 for Jupyter:

```
docker run -p 8889:8889 -it -v $PWD:/genc genc:latest /bin/bash
```

### 2. Launch the notebooks

Inside your running docker container instance, at the root of the repo, run:

```
cd genc
bash docs/tutorials/jupyter_setup/launch_jupyter.sh
```

Once fully initialized, the Jupyter server will print a link on the console
that you can use to connect to it. Open the link in your local browser on the
machine, on which you started the docker container (or tunnel port 8889 from
a remote machine and open the browser there).

## Android setup

Note: **DISCLAIMER**: Before we continue with the setup details, we'd like to
remind you that the GenC demo app is intended primarily for research and
experimental purposes, and uses in a non-experimental setting are at your own
risk. In particular, the model inference APIs used underneath may change any
time, and models used do not have any built-in safety mechanisms,
and may produce unsafe outputs in response to raw prompts.

Note: If you're planning to build a production mobile app to run on Android,
at this time we recommend that you review the [Gemini API](https://ai.google.dev/),
and the
[Gemini Nano on-device through Android AICore](https://android-developers.googleblog.com/2023/12/a-new-foundation-for-ai-on-android.html).
AICore is the new system-level capability introduced in Android 14 to provide
Gemini-powered solutions for high-end devices, including integrations with the
latest ML accelerators, use-case optimized LoRA adapters, and safety filters.
To start using Gemini Nano on-device with your app, apply to the
[Early Access Preview](https://docs.google.com/forms/d/e/1FAIpQLSdDvg0eEzcUY_-CmtiMZLd68KD3F0usCnRzKKzWb4sAYwhFJg/viewform?usp=header_link).

For running GenC on Android and GenC's Android demo app, see
setup instructions at [android_setup.md](genc/docs/android_setup.md).

Success: You have completed the setup of GenC.
Create your dream generative AI project, and share your experience with us!
