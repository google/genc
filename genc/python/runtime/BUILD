load("@rules_python//python:defs.bzl", "py_library", "py_test")

py_library(
    name = "runtime",
    srcs = ["__init__.py"],
    visibility = [
        "//genc:__subpackages__",
    ],
    deps = [
        ":runner",
        ":synchronous_context",
    ],
)

py_library(
    name = "runner",
    srcs = ["runner.py"],
    data = [
        "//genc/cc/authoring:constructor_bindings.so",
        "//genc/cc/intrinsics:intrinsic_bindings.so",
        "//genc/cc/runtime:executor_bindings.so",
    ],
    deps = [
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/base:to_from_value_proto",
    ],
)

py_test(
    name = "runner_test",
    srcs = ["runner_test.py"],
    deps = [
        ":runner",
        "//genc/python/authoring",
    ],
)

py_library(
    name = "synchronous_context",
    srcs = ["synchronous_context.py"],
    deps = [
        ":runner",
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/base",
    ],
)

py_test(
    name = "synchronous_context_test",
    srcs = ["synchronous_context_test.py"],
    deps = [
        ":synchronous_context",
        "//genc/python/authoring:tracing_context",
        "//genc/python/authoring:tracing_decorator",
    ],
)
