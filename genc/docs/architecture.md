# GenC Architecture

**GenC** is an open-source framework and an initiative
to facilitate building GenAI-powered applications in a composable manner, by
integrating diverse components across platforms and ecosystems and bringing them
together for use by application developers.

A deep notion of composability and customizability are at the core of GenC's
design, and this is reflected in across all layers of the system through modular
architecture and emphasis on extensibility.

The key components include:

*   **Intermediate Representation (IR)**, discussed in more detail in
    [ir.md](ir.md), forms the backbone of the platform. You can think of it as
    a simple functional DSL (domain-specific programming language) for
    expressing processing workflows. Abstractions in this DSL (such as model
    inference calls, prompt templates, custom functions, chains, etc.) match
    the level of granularity in common SDKs (such as LangChain), and the set of
    available abstractions is fully customizable and extensible (see the
    section on extensibility APIs in [api.md](api.md) on how you can define your
    own operators, including custom control flow operators).

*   **Modular runtime**, discussed in more detail in [runtime.md](runtime.md),
    is a set of C++ components for executing processing defined in the IR. The
    runtime is deeply configurable, and can be setup with lightweight set of
    dependencies, in a variety of platforms (in Linux environments (e.g. on
    servers), on mobile platforms, etc.), with your own custom backends,
    additional custom operators of your choosing, or some of the existing
    operators removed (for streamlining, or to avoid unwanted dependencies),
    etc., to match diverse use-cases and environments (see the section on
    extensibility APIs in [api.md](api.md)).

    The core of the runtime and the extensibility APIs are implemented in C++
    for performance and portability across platforms, but the runtime can be
    used from other programming languages (e.g., from Python via ```pybind11```,
    from Java via ```JNI```, etc., as shown in our examples).

*   **Development surface**, discussed in [api.md](api.md), facilitates
    expressing processing logic by the developers, converting it into IR, and
    submitting it for execution by the GenC runtime. As discussed in
    [api.md](api.md), we provide a few modes of authoring, from native APIs
    (e.g. C++, Python, Java) to interoperability with existing SDKs like
    LangChain.

*   **Interoperability layers** for frontends and backends that facilitate
    connecting GenC to a variety of existing systems. This includes frontend
    interoperability (e.g., to enable authoring of code in SDKs like LangChain,
    and other SDKs in the future), as well as backend interoperability (e.g.,
    to enable delegating execution to a variety of on-device and cloud models
    and other types of external services, including custom runtimes, etc.).
    This layer is rapidly evolving, and as such, we default to code being the
    best form of documentation (see the language-specific directories named
    `interop` and their contents, e.g., `python/interop/langchain` for interop
    with LangChain as a frontend, or `cc/interop/` for backend integrations).

When building a new system with GenC, the general flow is as follows:

*   Developer defines GenAI processing in a framework of their choosing among
    those we provide interop APIs for (e.g LangChain). This generally results in
    an object (e.g., a Python object that represents a chain in LangChain) that
    represents the processing to perform. Alternatively, or in addition, the
    developer might opt to use native authoring APIs, or to compose code authored
    in more than one SDKs. See [api.md](api.md) for more on authoring surfaces.

*   Developer uses the provided interop APIs to convert any code that is not yet
    in the IR form (e.g., Python objects created in LangChain) into a portable
    representation in the IR. See [api.md](api.md) for more on conversion.

*   Developer provisions a GenC runtime on their preferred platform. This can be
    as simple as using a one-liner constructor (e.g., using those we provide to
    run our examples and tutorials), or it can involve a custom setup that one
    can configure with their preferred backends, a curated set of dependencies,
    etc. See [api.md](api.md) for details on Runtime API, and see the section on
    Extensibility API for runtime customization. Also, see [runtime.md](runtime.md)
    for lower-level details if you wish to dig even deeper.

*   Developer passes the IR to the constructed runtime instance. Either locally,
    if authoring and execution happen on the same machine and in the same
    process, or else, the developer deploys the IR onto their target platform
    (e.g., a mobile phone), and loads it there against a runtime instance that
    was configured for that platform (as is done in several of the included
    tutorials).

*   The local runtime where the IR was loaded interprets the structure of the
    submitted computation, and either executes elements of it locally
    in-process, or if the computation is defined to be executed remotely and
    the runtime is setup for distributed delegation, it can delegate
    the execution of all or parts of the computation defined in the IR to
    instances of the runtime that exist remotely (e.g., from device to Cloud, to
    a trusted execution environment if supported, etc.).

GenC can scale to match the complexity of the desired deployment scenario. As
you can see in the included [tutorials](tutorials/), this can range from a few
lines of code in a Colab notebook environment, to slightly more when deploying
from Colab to e.g., a mobile app, and/or creating more complex workflows (as in
the [ReAct agent example tutorial](tutorials/tutorial_5_langchain_agent.ipynb)).
