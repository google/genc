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
    decision which model in the cascae to use by a custom routing
    function based on the on-device LLM evaluating the senstivity of
    the input query.

*   (Tutorial #3, place for a tutorial that shows agents with tools, including code sandbox
    and one other alternative tool)

*   (Tutorial #4, place for a tutorial that shows how to setup custom runtimes, define your
    own custom functions, alternative inference backends, add new operators to
    the system, etc.)

*   (Tutorial #5, and the last one planned for now - place for a tutorial focused on interoperability with LangChain)
