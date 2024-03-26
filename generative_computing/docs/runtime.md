# Generative Computing (GenC) Runtime

## Overview

Many of the key concepts behind the runtime are discussed in other parts of the
documentation; please review the sections on [architecture](architecture.md),
[IR](ir.md), and the [APIs](api.md) before proceeding. Most users do not need to
read this document, except if you plan to customize the runtime or build your
own replacement. In what follows, we will focus only on the internal runtime
concepts needed for the latter.

All execution is mediated by *executors*. An executor is a component that will
process the IR, orchestrate processing, interact with various backends as
needed, and return a result. While, per this description, an executor handles
execution end-to-end, it's actually a building block. Executors can be (and
generally are) organized into *executor stacks*, where different executors in
the stack handle different parts of processing and delegate to one-another,
as well as into distributed deployments, e.g., where an on-device executor in
a Java app might delegate processing to an executor instance in cloud. Usually
an executor is local, but in some cases it can be remote, in a different
process, or on a different physical hardware elsewhere in the nework.

Interaction with an executor is mediated by the `ExecutorInterface` interface
defined in C++ in [cc/runtime/executor.h](../cc/runtime/executor.h). This API is
fundamental, in that this is how an app talks to an executor, how executors
talk to one-another, and it's an API offered to custom operator implementers to
enable them to define, e.g., new control flow operators.

You can think of an *executor* as essentially an abstract "box" inside of
which you can statically or dynamically orchestrate processing by "pulling the
strings" from the outside through handles that point to objects inside of it:

*   The `CreateValue` call allows you to put something into the box, so that it
    can be used for processing. This something can be a number, a string, but
    it can also be a function represented by a piece of IR (see [ir.md](ir.md)
    for more on this).

    The call returns a handle to the created value, with asynchronous future
    semantics. What this means is that by the time you receive that handle,
    which normally is instantaneous, processing may or may not have even
    started, but the handle can be immediately used as an argument to another
    call, thus allowing you to dynamically construct processing pipelines while
    the execution is taking place asynchronously. The same holds for handles
    returned by all the subsequent calls (except where otherwise noted in case
    of `Materialize`).

*   The `CreateCall` call allows you to take a pair of things in the box (that
    are represented by a pair of handles), one of which will be treated as a
    function, and another of which will be treated as an argument, and initiate
    a function call, which, again, will be processed asynchronously.

    The call returns a handle to the (asynchronous future) result of the
    function call that, again, can be immediately used as an argument to
    another call, even if it has not materialized yet.

*   The `CreateStruct` and `CreateSelection` calls allow you to form structures
    and select elements from structures among things in the box. Similar to
    the above, they take and return handles with asynchronous future semantics.

*   The `Materialize` call takes a handle to something in the box,
    synchronously waits until the corresponding object (e.g., result of some
    processing) has been computed, and returns the associated payload (e.g.,
    a result string if it's an LLM inference call). Unlike the 4 preceding
    calls listed above, this call is blocking (since it has to wait for the
    actual processing, potentially long-running, to finish). This call applies
    to only the specified object. One can issue this call on anything in the
    box, potentially multiple times, and continue to add things to the box to
    continue scheduling more computation in parallel.

*   The `Dispose` call releases resources associated with a handle (to a thing
    in the box). In most cases, callers don't need to manage lifetime of values
    explicitly, since it's taken care of by the supplied wrapper classes such as
    `OwnedValueId` that call `Dispose` on your behalf (unless a more elaborate
    lifetime management system is needed).

The above provides flexibility to control the granularity of processing passed
to an executor, to either submit all of it at once (in a single `CreateValue`
that is subsequently `Materialize`'d to compute it), or to interact with an
executor in what one might think of as a sort of *eager mode*, where existing
processing can be arbitrarily dynamically appended to and expanded while it is
underway. The choice of one vs. the other is up to the developer.

## Executors

As noted above, executors are designed to be used as components, and composed
into *executor stacks*. GenC comes with a pair of executors designed to support
basic usage - the `ControlFlowExecutor` (see
[cc/runtime/control_flow_executor.h](../cc/runtime/control_flow_executor.h)),
and `InlineExecutor` (see
[cc/runtime/inline_executor.h](../cc/runtime/inline_executor.h)). The former
is responsible for handling all types of control flow processing, including
e.g., lambda expressions, as well as all types of custom control flow operators.
The latter is responsible for handling data in-and-out type of processing
such as, e.g., model inference calls, prompt templates, and other types of
inline operators. Normally, the two work together. The `CreateLocalExecutor`
call creates a combo, with the former delegating to the latter. Normally, this
will be sufficient for most uses. You may, however, potentially wish to
customize this setup, e.g., to allow control flow and data in-and-out type of
processing to happen on different machines, or to leverage your own specialized
workflow or pipelining platform. The separation of concerns between these two
executors aims to make this easier: the control flow executor handles most of
the complexity, thus allowing inline executor to be short and compact, and used
as a potential example template for implementing new types of custom executors.

## Handlers

Executors included with GenC implement all core abstractions, but when presented
with an *intrinsic* (custom operator), they need to delegate processing, since
these are fully configurable by the user. This is accomplished via *intrinsic
handlers* that abstract away the logic, and can be added or removed (as covered
in the extensibility section in [api.md](api.md)). The two supplied executors
recognize the type of intrinsics they are responsible for, and delegate to an
appropriate handler (as defined in the *handler set* included during the runtime
construction).
