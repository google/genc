# Generative Computing (GenC) Architecture

**Generative Computing** (GenC) is an open-sourced framework and an initiative
to facilitates building GenAI-powered applications in a composable manner, by
integrating diverse components across platforms and ecosystems and bringing them
together for use by application developers.

A deep notion of composability and customizability are at the core of GenC's
design, and this is reflected in across all layers of the system through modular
architecture and emphasis on extensibility.

The key components include:

*   **Intermediate Representation (IR)**, discussed in more detail in
    [ir.md](ir.md), forms the backbone of the platform. You can think of it as
    a simple functional DSL for expressing processing workflows. Abstractions
    in this DSL match the level of granularity in common SDKs (like LangChain),
    and the set of available abstractions is customizable and extensible (see
    the section on extensibility APIs in [api.md](api.md)).

*   **Modular runtime**, discussed in more detail in [runtime.md](runtime.md),
    is a set of C++ components for executing processing defined in the IR. The
    runtime is deeply configurable, and can be setup with lightweight set of
    dependencies on a variety of platforms (in Linux environments, on Android,
    etc.), with your own custom backends, additional custom operators of your
    choosing, or some of the existing operators removed (for streamlining, or
    to avoid unwanted dependencies), etc., to match diverse environments (see
    the section on extensibility APIs in [api.md](api.md)). The core of the
    runtime and the extensibility APIs are implemented in C++, but the runtime
    can be used from other languages (e.g., from Python via `pybind11``, from
    Java via `JNI`, etc., as shown in our examples).

*   **Development surface**, discussed in [api.md](api.md) that facilitates
    expressing processing logic by the developers, converting it into IR, and
    submitting for execution. As discussed in [api.md](api.md), we provide a few
    modes of authoring, from native APIs to interability with existing SDKs like
    LangChain.

*   **Interoperability layers** for frontends and backends that facilitate
    connecting GenC to a variety of existing systems. This includes frontend
    interability (e.g., to enable authoring of code in SDKs like LangChain),
    as well as backend interoperability (e.g., to enable delegating execution
    to a variety of on-device and cloud models, external services, etc.).
    This layer is rapidly evolving, and as such, we default to code being the
    best form of documentaiton; see the language-specific directories named
    `interop` and their contents.

The general flow is as follows:

*   Customer defines GenAI processing in a framework of their choosing among
    those we provide interop APIs for (e.g LangChain). This generally results in
    an object (e.g., a Python object that represnets a chain in LangChain) that
    represents the processing to perform.

*   Customer use the provided interop APIs to convert the Python object into a
    portable representation in the IR, and either hands it over to a locally
    constructed runtime instance, or deployes it on a different platform (e.g.,
    on a mobiel phone), and loads it there.

*   The local runtime where the IR was loaded interprets the structure of the
    submitted computation and either executes elements of it locally in-process,
    or if the runtime is setup for distributed delegation, it can delegate the
    execution of all or parts of the computation defined in th IR to instances
    of the runtime that exist remotely (e.g., from device to Cloud, to a trusted
    execution environment if supported, etc.).

See the included [tutorials](tutorials/) for examples of this pattern.
