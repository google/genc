# Generative Computing (GenC) Tutorials and Demos

What you can find in this directory:

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

*   [**Tutorial 6.** Port Agent to Android](tutorial_6_android_agent.ipynb).
    This tutorial builds on all the above, and shows how an agent with tools
    just created can be ported to run in a Java app on Android.
