load("@rules_python//python:defs.bzl", "py_library", "py_test")

py_library(
    name = "runtime",
    srcs = ["__init__.py"],
    visibility = ["//generative_computing/python:python_packages"],
    deps = [
        ":runner",
    ],
)

py_library(
    name = "runner",
    srcs = ["runner.py"],
    data = ["//generative_computing/cc/runtime:executor_bindings.so"],
    deps = [
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/proto/v0:executor_py_pb2",
    ],
)

py_test(
    name = "runner_test",
    srcs = ["runner_test.py"],
    deps = [
        ":runner",
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/python/authoring",
    ],
)
