# Generative Computing Design

## Overview

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

*   Customer defines genAI processing in a framework of their choosing among
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

## Client-facing APIs

### LangChain

In order to facilitate easy on-ramp for customers who are currently using
[LangChain](https://www.langchain.com/) APIs, we aim to support roughly same
Python APIs, e.g., customers can define Python objects that represent chains,
and submit these in through our interop APIs for execution. We also bring in
necessary components such as cascading to compliment what LangChain doesn't
have.

We provide a Python API `interop.langchain.create_computation()` that accepts an
existing LangChain Python object, and maps it into an instance of
`computation.proto`, potentially given additional arguments to control hybrid
on-device and Cloud processing. The resulting computation can subsequently be
executed using generic runtime APIs. Note that in this case, LangChain is not
used to orchestrate the execution of the initial Python object. That object is
translated into a Generative Computing computation structure and subsequently
orchestrated and flexibly deployed to hybrid environment.

TODO(b/295260921): Document here a system for naming these models and how the
URIs are formed. Also available building blocks.

## Runtime

The runtime itself is modular, structured as a tree of *executors* each of which
specializes in processing some subset of computation structures (e.g., only
model calls, or chain-like control flows), interfaces a particular kind of a
runtime environment (e.g., system services on Android), or handles delegation to
remote runtime instances.

TODO(b/295015950): Document the rest of this.
