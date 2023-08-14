# Generative Computing Design

## General overview

The framework consists of roughly the following key components:

*   `computation.proto` that intends to model platform- and language-independent
    representations of genAI workflows that can be deployed in a variety of
    environments. The proto is design to support common abstractions, such as
    those used in [LangChain](https://www.langchain.com/) and similar platforms.

*   Runtime that can execute instances of `computation.proto` and has a modular
    structure that enables it to be deployed in and configured for a variety of
    environments, including on-device, in Cloud, in trusted execution
    environments, and hybrid deployments that might span across and combine all
    of the above.

*   Interop APIs that enable this framework to interoperate with a variety of
    other systems, notably the various frontends (e.g., LangChain).

The general flow is as follows:

*   Customer defines genAI processing in a framework of their choosing among
    those we provide interop APIs for (currently LangChain). This generally
    results in a Python object (e.g., a chain in LangChain) that represents the
    processing to perform.

*   Customer the provided interop APIs to convert the Python object into a
    portable representation (`computation.proto`), and hands it over to a local
    runtime instance.

*   The local runtime interprets the structure of the submitted computation and
    either executes elements of it locally in-process (which can be on-device),
    or delegates execution of parts of the computation to remote runtime
    instances in other processes, or elsewhere on the network (e.g., in Cloud,
    in a trusted execution environment, etc.).

## Client-facing APIs

### LangChain

In order to facilitate easy on-ramp for customers who are currently using
[LangChain](https://www.langchain.com/) APIs, we aim to support roughly same
Python APIs, e.g., customers can define Python objects that represent chains,
and submit these in through our interop APIs for execution.

We provide a Python API `interop.langchain.create_computation()` that accepts an
existing LangChain Python object, and maps it into an instance of
`computation.proto`, potentially given additional arguments to control hybrid
on-device and Cloud processing. The resulting computation can subsequently be
executed using generic runtime APIs. Note that in this case, LangChain is not
used to orchestrate the execution of the initial Python object, since that
object is being translated into a Generative Computing computation structure and
subsequently orchestrated by this framework.

Current limitations and peculiarities:

*   We only support `interop.langchain.CustomModel` as an LLM, with the exact
    type of model and its version captured as a URI.

TODO(b/295260921): Document here a system for naming these models and how the
URIs are formed.

## Runtime

The runtime itself is modular, structured as a tree of *executors* each of which
specializes in processing some subset of computation structures (e.g., only
model calls, or chain-like control flows), interfaces a particular kind of a
runtime environment (e.g., system services on Android), or handles delegation to
remote runtime instances. Customers can use the runtime APIs to construct custom
executor stacks.

TODO(b/295015950): Document the rest of this.
