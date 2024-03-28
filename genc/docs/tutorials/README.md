# GenC Tutorials and Demos

## Tutorial Overview

*   [**Tutorial 1.** Simple chain in LangChain backed by a device-to-Cloud cascade](tutorial_1_simple_cascade.ipynb).
    This tutorial shows how to define simple application logic in LangChain, use
    our interop APIs to configure it to be powered by a cascade of models that
    spans across a model in Cloud and an on-device model on Android, and deploy
    it in a Java app on an Android phone. This illustrates many of the key
    interoperability and portability benefits of GenC in one concise package.
    See the follow-up tutorials listed below for how you can further extend and
    customize such logic to power more complex use cases.

*   [**Tutorial 2.** Custom model routing function based on a scorer](tutorial_2_custom_routing.ipynb).
    This tutorial builds on the preceding one, and shows how to drive the
    decision which model in the cascade to use by a custom routing function
    based on the on-device LLM evaluating the sensitivity of the input query.

*   [**Tutorial 3.** Custom Runtime & Tools](tutorial_3_custom_runtime.ipynb).
    This tutorial shows how the runtime itself can be customized to support
    your applications's needs, e.g., by defining custom functions, adding new
    model backends, or even new types of operators.

*   [**Tutorial 4** Math Tool Agent](tutorial_4_math_tool_agent.ipynb).
    This tutorial builds on the preceding one, and show how you can use the
    custom functions you created to express a ReAct reasoning loop that can
    power agents and tool use.

*   [**Tutorial 5.** LangChain Agent](tutorial_5_langchain_agent.ipynb).
    This tutorial further builds on the above, this time focusing on
    interoperability with LangChain. It demonstrates how with GenC building
    blocks, you can build agent-based applications that combine LangChain API
    for authoring with a robust C++ runtime for deployment.

## Running Tutorials in a Jupyter Notebook
### Using the GenC Docker Container
If you are using the GenC Docker container, then the prerequesites for running the notebook server are already installed. You'll just need to start the Docker container with the appropriate port (`8889`) forwarded to the host. For example, to forward the port and mount a local model directory:

```
docker run -it -v <GenC root>:/genc -v <local models directory>:/models -p 8889:8889 <Docker container tag> /bin/bash
```

Once in the container, you can run the notebook server:

```
cd genc
bazel run genc/docs/tutorials/jupyter_setup:jupyter
```

Once the server starts, the console will include information on a URL that includes an access token (otherwise you'll need to configure a password). For example:

```
    To access the server, open this file in a browser:
        file:///root/.local/share/jupyter/runtime/jpserver-13-open.html
    Or copy and paste one of these URLs:
        http://dce025832b6d:8889/tree?token=687930d73e648c4e127ae4c585f3a3f578d0611b885a301d
        http://127.0.0.1:8889/tree?token=687930d73e648c4e127ae4c585f3a3f578d0611b885a301d
```

Because the port is forwarded to the host machine, you can use the last URL with the hostname changed (e.g. `http://localhost:8889/tree?token=687930d73e648c4e127ae4c585f3a3f578d0611b885a301d`)

### Without Docker
Without Docker the steps are fairly similiar but you'll need to first install the prerequisites. We've included a script that installs the prerequsities (Debian and Python deps) and then runs the server. From the GenC root:

```
bash genc/docs/tutorials/jupyter_setup/launch_jupyter.sh
```

After execution the server can be accessed with the URLs+tokens as described in [Using the GenC Docker Container](#using-the-genc-docker-container).
