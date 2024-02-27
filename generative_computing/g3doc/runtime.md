# Generative Computing (GenC) Runtime

## Overview

Many of the key concepts behind the runtime are discussed in other parts of the
documentation; please review the sections on [architecture](architecture.md),
[IR](ir.md), and the [APIs](api.md) before proceeding. Most users do not need to
read this document, except if you plan to customize the runtime or build your
own replacement. In what follows, we will focus only on the internal runtime
concepts needed for the latter.

All execution is mediated by *executors*. An executor is a component that will
process the IR, orchestrate processing, interact with various backend as
needed, and return a result. While, per this description, an executor handles
execution end-to-end, it's actually a building block. Executors can be (and
generally are) organized into *executor stacks*, where different executors in
the stack handle different parts of processing and delegate to one-another,
as well as into distributed deployments, e.g., where an on-device executor in
a Java app might delegate processing to an executor instance in cloud. Usually
an executor is local, but in some cases it can be remote, in a different
process, or on a different physical hardware elsewhere in the nework.

Interaction with an executor is mediated by the `ExecutorInterface` interface
defined in C++ in [cc/runtime/executor.h](cc/runtime/executor.h). This API is
fundamental, in that this is how an app talks to an executor, how executors
talk to one-another, and it's an API offered to custom operator implementers to
enable them to define, e.g., new control flow operators.

You can think of an *executor* as essentially as an abstract box in which you
can statically or dynamically orchestrate processing:

*   The `CreateValue` call allows you to put something into the box, so that it
    can be used for processing. This something can be a number, a string, but
    it can also be a function represented by a piece of IR (see [ir.md](ir.md)).

    The call returns a handle to the created value, with asynchronous future
    semantics. What this means is that by the time you receive that handle,
    which normally is instantaneous, processing may not have even started, but
    the handle can be immediately used as an argument to another call, thus
    allowing you to dynamically construct processing pipelines. The same holds
    for handles returned by all the subsequent calls (except where otherwise
    noted in case of `Materialize`).

*   The `CreateCall` call allows you to take a pair of things in the box (that
    are represented by a pair of handles), one of which will be treated as a
    function, and another of which will be treated as an argument, and initiate
    the function call (which will begin asynchronously).

    The call returns a handle to the (asynchronous future) result of the
    function call.

*   The `CreateStruct` and `CreateSelection` calls allow you to form structures
    and select elements from structures among things in the box.

*   The `Materialize` call takes a thing in the box, synchronously waits until
    that thing is computed, and returns the associated payload (e.g., a result
    string). Unlike the 4 preceding cals listed above, this call is blocking.

*   The `Dispose` call releases resources associated with a handle (to a thing
    in the box). In most cases, callers don't need to manage lifetime of values
    explicitly, since it's taken care of the supplied wrapper classes (unless a
    more elaborate lifetime management system is needed).

The above provides flexibility to control the granularity of processing passed
to an executor, to either submit all of it at once (in a single `CreateValue`
that is subsequently `Materialize`d to compute it), or to interact with an
executor in what one might think of as a sort of *eager mode*, where existing
processing can be arbitrarily dynamically appended to and expanded while it is
underway.
