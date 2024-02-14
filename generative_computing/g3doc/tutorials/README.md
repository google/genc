# Generative Computing (GenC) Tutorials and Demos

What you can find in this directory:

*   [**Tutorial 1.** Simple chain in LangChain powered by a device-to-Cloud cascade]
    (tutorial_1_simple_cascade.ipynb).
    This tutorial shows how to define simple application logic in LangChain,
    use our interop APIs to configure it to be powered by a cascade of models
    that spans across a model in Cloud and an on-device model on Android,
    and deploy it in a Java app on an Android phone. This illustrates many of
    the key interoperability and portability benefits of GenC in one concise
    package. See the follow-up tutorials listed below for how you can further
    extend and customize such logic to power more complex use cases.

*   [**Tutorial 2.** Custom model routing function based on a scorer]
    (tutorial_2_custom_routing.ipynb).
    This tutorial builds on the one above, and shows how to drive the
    decision which model in the cascade to use by a custom routing
    function based on the on-device LLM evaluating the sensitivity of
    the input query.

*   [**Tutorial 3.** Custom Runtime & Tools](tutorial_3_custom_runtime.ipynb).
    GenC can be customized to become your own SDK. This is a more advanced
    tutorial to illustrate how to setup custom runtime, define your own custom
    functions, and add new operators to the system.

*   [**Tutorial 4.** Math Tool Agent](tutorial_4_math_tool_agent.ipynb). This
    tutorial builds on Tutorial 3, and show how you can combine ReAct reasoning
    loop, tool use, with the custom operators you created.

*   [**Tutorial 5.** LangChain Agent](tutorial_5_langchain_agent.ipynb). This
    tutorial builds on Tutorial 4, it's focused on interoperability with
    LangChain. It demonstrates with GenC building blocks, you can combine
    LangChain interface and robust C++ runtime from GenC.
