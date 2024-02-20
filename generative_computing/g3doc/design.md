# Generative Computing Design

**Generative Computing** (GenC) is a framework that enables building
GenAI-powered applications. It's developed by Google. Its core engine functions
as a glue piece (Intermediate Representation or IR) between a diverse GenAI
landscape. And therefore is able to connect a variety of frontend and backend
systems. Its core consists of the following components:

*   *`interops`*, that is able to connect to a variety of frontend and backend
    systems. It works by adapting various requests, responses, and configs into
    a standard format supported by GenC runtime. It's also backed by
    light-weight backend integrations.
*   *`computation.proto`*, which is able to express arbitrary computation or
    GenAI workflow. And multimodal `value proto`, that is able to carry
    information between computation steps, such as tensor, string, image, audio,
    tool responses etc. The protos are design to support common abstractions,
    such as those used in [LangChain](https://www.langchain.com/) and similar
    platforms.
*   *C++ Runtime*, that can run arbitrary `computation.proto`, powered by
    production tested learning from hybrid ML and distributed ML. It has a
    modular structure that enables it to be deployed in and configured for a
    variety of environments, including on-device, in Cloud, in trusted execution
    environments, and hybrid deployments that might span across and combine all
    of the above.

The general flow is as follows:

*   Customer defines GenAI processing in a framework of their choosing among
    those we provide interop APIs for (e.g LangChain). This generally results in
    a Python object (e.g., a chain in LangChain) that represents the processing
    to perform.

*   Customer use the provided interop APIs to convert the Python object into a
    portable representation (`computation.proto`), and hands it over to a local
    runtime instance.

*   The local runtime interprets the structure of the submitted computation and
    either executes elements of it locally in-process (which can be on-device),
    or delegates execution of parts of the computation to remote runtime
    instances in other processes, or elsewhere on the network (e.g., in Cloud,
    in a trusted execution environment, etc.).
