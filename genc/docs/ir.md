# GenC's Intermediate Representation (IR)

## Overview

The Intermediate Representation (IR) is at the core of GenC architecture - it's
what enables the composability, portability, flexible deployment, and hybrid
(device + Cloud) computing benefits.

The structure of the IR is defined by the protocol buffer message named
`Value`, you can find it in [computation.proto](../proto/v0/computation.proto).
You can think of this protocol buffer as effectively defining a simple
domain-specific language (DSL), with the `Value` being a recursive structure
that represents the processing to be performed.

The language of GenC's IR is based on the functional paradigm, meaning that
functions are first-class objects that can be passed as arguments and returned
as results. This is reflected in the name `Value` chosen for the structure that
represents processing. As you can see from the proto definition, instances of
this message (`Value`) can include simple data types (such as `int32`, `string`,
`bool`, `float`, as well as tensor types, etc.), but also structures that serve
as building blocks for what we'll call *computations* (processing).

The building blocks for defining computations include 7 core abstractions, and
an extensible system of pluggable operators, covered in the following two
sections.

## Core Abstractions

GenC IR comes with a fixed set of 7 built-in core abstractions. These are the
only built-in abstractions included, and they're always available.

### Function Call

Message `Call` (variant `call` in `Value`) represents a function call, similar
to that offered by most programming languages. It takes two arguments - the
first represents the function to be invoked, and the second represents the
argument to pass to that function. Both the function and the argument are
defined in the IR.

Here's an example piece of IR that uses the function call, with the model
inference (here to the Gemini model in cloud) representing the function being
invoked, and the argument in this case being embedded in the IR as a string.
Note that in a more realistic setting, the argument would come from the user,
or produced as an output of another piece processing - here we keep it simple
for demonstration purposes.

```
call {
  function {
    intrinsic {
      uri: "model_inference" static_parameter { str: "/cloud/gemini" }
    }
  }
  argument {
    str: "hello"
  }
}
```

### Function Definition

Message `Lambda` (variant `lambda` in `Value`) represents a lambda expression,
or in simple terms, an inline definition of a function (similar to `lamda` in
Python, or the lambda expressions in C++11). It takes the name of the parameter
declared by the function, and a `result` IR that represents the function body
and the result to be computed. Within the function body, the formal parameter
can be referred to via the `Reference` abstraction discussed below.

Here's an example piece of IR that uses the function definition, with the
parameter named `arg`, and a body feeding it to a model inference call like the
one you've seen above as input. Now, this is again a simplistic example for
the sake of illustration. See the
[custom routing tutorial](tutorials/tutorial_2_custom_routing.ipynb)
for a more elaborate example of processing for a more realistic example of use
of this abstraction.

```
lambda {
  parameter_name: "arg"
  result {
    call {
      function {
        intrinsic {
          uri: "model_inference" static_parameter { str: "/cloud/gemini" }
        }
      }
      argument {
        reference { name: "arg" }
      }
    }
  }
}
```

### Parameter Reference

Message `Reference` represents a reference to a named formal parameter, e.g.,
in the body of a function definition. See above for example usage. References
can also be used to define named aliases in *let expressions* (see below).
When nested, they follow the usual rules of naming (name defined within nested
scope hides a name defined in the surrounding scope).

### Structure

Message `Struct` defines a *structure*, or simply a tuple with an ordered set
of elements. A tuple is often used to assemble a set of values to form an
argument to a function call.

Here's an example piece of IR that defines an ordered tuple of two numbers.

```
struct {
  element { int_32: 10 }
  element { int_32: 20 }
}
```

### Selection

Message `Selection` defines a selection of an element from a structure (tuple).
A selection is often used to refer to one of the elements of a complex argument
or to one of the outputs of a function call, in cases where the function takes
multiple arguments or returns multiple results (which is always formally modeled
in the IR as taking or returning a struct). Structures and selections, among
other things, are essentially, a way of routing results between function calls
in a processing workflow.

Here's an example piece of IR that selects the fourth element from a tuple that
is represented by the argument `arg`.

```
selection {
  source {
    reference {
      name: "arg"
    }
  }
  index: 3
}
```

### Let Expression

Message `Block` defines a block of processing, and is essentially a form of a
*let expression*, i.e., an expression that first defines a set of named *local
values* (or simply *locals*), and then uses those values within the final
expression, where they can be accessed in a manner similar to a formal function
parameter (via `Reference`).

Technically redundant, it has a role in making IR simpler and more compact, and
enables the IR to more or less follow the same flow as what one would encounter
in a more typical programming language.

Here's an example piece of IR that defines two local values `func` and `arg`,
and then uses them in the body of the final expression (which is a simple
function call). Like many example above, this one is simplistic, merely for
illustration purposes. You will find more complex uses of this abstraction in
many of the examples uses in the tutorials, albeit it will be implicit, with
blocks of processing constructed under the hood by the various helper functions
defined in GenC authoring libraries, rather than in the tutorial code.

```
block {
  local {
    name: "func"
    value {
      intrinsic {
        uri: "model_inference" static_parameter { str: "/cloud/gemini" }
      }
    }
  }
  local {
    name: "arg" value { str: "hello" }
  }
  result {
    call {
      function { reference { name: "func" } }
      argument { reference { name: "arg" } }
    }
  }
}
```

### Custom Operators

The last of the 7 abstractions are pluggable *custom operators*, also called
*intrinsics*, defined by the `Intrinsic` message. GenC comes with a library of
basic operators, some of which you've seen in examples listed above (e.g.,
model inference calls), but the system is designed to make these operators
plug and play, and the associated runtime fully modular. As such, none of the
operators included with GenC are mandatory, or in any way special. You can
set up a GenC-based environment that includes your own set of operators, and
it's up to you to decide which of the operators supplied by GenC, if any, will
be included in that setup and loaded by the runtime.

As you saw in examples above, instances of `intrinsic` in the IR always include
the *URI* string, which uniquely identifies the custom operator, and have an
optional *static parameter* embedded within the IR along with the operator. The
accepted forms and the meaning of that static parameter are entirely dependent
on the operator (i.e., they're the function of the *URI*). Some operators, such
as `model_inference`, may just take a string (the model name), whereas others
may take a struct with a number of different elements, potentially nested.

## System of Pluggable Operators

As noted above, the system of pluggable operators is modular and customizable.
The set of operators included in the GenC repo can be found in
[cc/intrinsics](../cc/intrinsics/), with the documentation contained in the
header file [intrinsic_uris.h](../cc/intrinsics/intrinsic_uris.h).

In order to promote the use of these operators in user-configured runtimes,
by default all of them will be wired into the runtime by the function
`CreateCompleteHandlerSet` declared in a neighboring [handler_sets.h](../cc/intrinsics/handler_sets.h),
albeit this is also fully configurable. See the discussion of extensibility APIs
 in the [api.md](api.md) for how to use it when setting up a custom runtime.


