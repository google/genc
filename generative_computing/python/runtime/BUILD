load("@rules_python//python:defs.bzl", "py_library", "py_test")

py_library(
    name = "runtime",
    srcs = ["__init__.py"],
    visibility = [
        "//generative_computing:__subpackages__",
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
        "//generative_computing/cc/authoring:constructor_bindings.so",
        "//generative_computing/cc/intrinsics:intrinsic_bindings.so",
        "//generative_computing/cc/runtime:executor_bindings.so",
    ],
    deps = [
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/proto/v0:executor_py_pb2",
        "//generative_computing/python/base:to_from_value_proto",
    ],
)

py_test(
    name = "runner_test",
    srcs = ["runner_test.py"],
    deps = [
        ":runner",
        "//generative_computing/python/authoring",
    ],
)

py_library(
    name = "synchronous_context",
    srcs = ["synchronous_context.py"],
    deps = [
        ":runner",
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/python/base",
    ],
)

py_test(
    name = "synchronous_context_test",
    srcs = ["synchronous_context_test.py"],
    deps = [
        ":synchronous_context",
        "//generative_computing/python/authoring:tracing_context",
        "//generative_computing/python/authoring:tracing_decorator",
    ],
)
