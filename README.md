# GenC

**GenC** is an open-source framework for building GenAI-powered applications.

GenAI applications often involve LLMs, chains, agent logic, custom functions,
RAG, ReAct, model cascades and routers, and other types of neural and
non-neural components connected into complex workflows.

Benefits we provide:

*   Composability & flexibility:

    *   Ability to freely compose building blocks to express your own
        application logic.
    *   Ability to integrate diverse SDKs, platforms, and ecosystems
        (e.g., LangChain, Android, cloud services, databases, models, etc.).
    *   Ability to express processing that combines on-device/on-prem and
        cloud components.

*   Portability & platform independence:

    *   Across programming languages (e.g., from Python to Java and C++).
    *   Across prototyping and production deployments (e.g., from Colab
        notebooks, to cloud servers, to mobile apps).
    *   Across on-device and cloud platforms (e.g., Linux, Android, Chrome,
        iOS, etc.).
    *   Can be configured with minimal dependencies for lightweight deployments
        (e.g, on embedded/IoT and other specialized platforms).

*   Customizability:

    *   Ability to incorporate your own libraries and network services (models,
        databases, etc.) as GenC building blocks, and mix and match them with
        those that already exist.
    *   Ability to create your own custom runtimes.

*   Development velocity:

    *   Building blocks allow logic to be expressed succinctly, at high level,
        and easily modified.
    *   Portability enables prototype code to be deployed with minimal effort
        to production environments.

*   Performance & security:

    *   C++ runtime (faster, more secure than Python).
    *   Asynchronous and parallel functional programming model.
    *   Application logic, represented in a declarative form, can be statically
        analyzed, verified, optimized, and executed on scalable distributed
        platforms.

Our target audience is any genAI developers, especially those seeking benefits
such as composability, portability, and deep customizability.

For more information, see the following:

*   [Tutorials](generative_computing/docs/tutorials/README.md) show diverse
    examples of usage to showcase some of GenC's capabilities.

*   [API documentation](generative_computing/docs/api.md) cover the developer
    and extensibility surfaces.

*   [Model documentation](generative_computing/docs/models.md) lists the
    supported models.

*   [Architecture](generative_computing/docs/architecture.md),
    [IR](generative_computing/docs/ir.md), and
    [runtime](generative_computing/docs/runtime.md) cover more advanced topics
    for a deeper level of customization.

*   [Setup](SETUP.md) instructions explain how to setup a full development
    and runtime environment that you can use to build GenC, run and customize
    the included examples (at this time, we only support building from source
    in GitHub; additional support for things like PIP releases, pre-built
    images, etc., will be included in the future).

*   [Contributors](CONTRIBUTING.md) instructions explain where and how you can
    contribute to the platform. GenC is designed to be extensible, and we'd
    like to grow it, with your help, to support your preferred domain and the
    kinds of services and capabilities you need. We welcome your contributions!

Use [GitHub issues](https://github.com/google/generative_computing/issues) for
tracking requests and bugs.

Please direct questions to [Stack Overflow](https://stackoverflow.com) using the
[generative-computing](https://stackoverflow.com/questions/tagged/generative-computing)
tag.
