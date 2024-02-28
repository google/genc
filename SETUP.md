# Setup Generative Computing

Quick and easy way to setup environment for GenC is using Docker Container:

### 1. Clone git repository

```
git clone https://github.com/google/generative_computing
```

### 2.Build Docker Container

```
cd generative_computing
docker build -t genc .
docker run -it -v $PWD:/generative_computing genc:latest /bin/bash -c "cd generative_computing; bash ./setup_android_build_env.sh; /bin/bash"
```

Note: There will be a (y/N) prompt to accept Android SDK, NDK license as part
of docker run, please accept the license. Check if you have successfully built
docker container. Check the active images by running "docker images".

### 3. Test and run examples

```
bazel test generative_computing/...
```

You can navigate to python/examples and pick your favorite example and run

```
bazel run python/examples:$favorite_example
```

## Access Genc tutorials with a Jupyter notebook

Building a Jupyter notebook in docker container has some extra setup to follow.
But it can be useful when you want to quickly test few things.

### 1. Add open port to docker.

Exit the docker container and connect to a open port

```
docker run -p 8889:8889 -it -v $PWD:/generative_computing genc:latest /bin/bash
```

### 2. Launch the notebooks

```
bash docs/tutorials/jupyter_setup/launch_jupyter.sh
```

Open the link in your local browser.

Success: You have completed the setup of Generative_computing.
Create your dream generative AI project, and share your experience with us!
