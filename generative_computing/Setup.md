# Setup Generative Computing

Quick and easy way to setup environment for GenC is using Docker Container:

### 1. Clone git repository

<pre class="prettyprint lang-bsh">
<code class="devsite-terminal">git clone https://github.com/google/generative_computing</code>
</pre>

### 2.Build Docker Container
<pre class="prettyprint lang-bsh">
<code class="devsite-terminal">cd generative_computing </code>
<code class="devsite-terminal">docker build -t genc . </code>
<code class="devsite-terminal">docker run -it -v $PWD:/generative_computing genc:latest /bin/bash -c "cd generative_computing; bash ./setup_android_build_env.sh; /bin/bash" </code>
</pre>

Note: There will be a (y/N) prompt to accept Android SDK, NDK license as part of docker run, please accept the license. Check if you have successfully built docker container. Check the active images by running "docker images".

### 3. Test and run examples

<pre class="prettyprint lang-bsh">
<code class="devsite-terminal"> bazel test generative_computing/... </code>
</pre>
####
   you can navigate to python/examples and pick your favorite example and run

<code class="devsite-terminal">bazel run python/examples:$favorite_example </code>


## Access Genc with a Jupyter notebook

Building a Jupyter notebook in docker container has some extra setup to follow. But it can be useful when you want to quickly test few things.


### 1. Install dependencies for Jupyter notebook.

<pre class="prettyprint lang-bsh">
<code class="devsite-terminal">apt install jupyter-notebook</code>
<code class="devsite-terminal">pip install -r python/examples/colabs/pip_jupyter.txt</code>

</pre>

### 2. Add open port to docker.

Exit the docker container and connect to a open port

<code class="devsite-terminal">docker run -p 8889:8889 -it -v $PWD:/generative_computing genc:latest /bin/bash </code>


### 3. Launch the notebook

<pre class="prettyprint lang-bsh">
<code class="devsite-terminal">bazel run python/example/colabs:jupyter </code>
</pre>

####Open the link in your local browser.

Success:You have completed the setup of Generative_computing. Create your dream generative ai project and share us your experience.
