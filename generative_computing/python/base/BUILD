load("@rules_python//python:defs.bzl", "py_library", "py_test")

py_library(
    name = "base",
    srcs = ["__init__.py"],
    visibility = ["//generative_computing/python:python_packages"],
    deps = [
        ":computation",
        ":context",
    ],
)

py_library(
    name = "computation",
    srcs = ["computation.py"],
    visibility = [
        "//generative_computing/python:python_packages",
    ],
    deps = [
        ":context",
        "//generative_computing/proto/v0:computation_py_pb2",
    ],
)

py_test(
    name = "computation_test",
    srcs = ["computation_test.py"],
    deps = [
        ":computation",
        ":context",
        "//generative_computing/proto/v0:computation_py_pb2",
    ],
)

py_library(
    name = "context",
    srcs = ["context.py"],
    visibility = [
        "//generative_computing/python:python_packages",
    ],
    deps = [
        "//generative_computing/proto/v0:computation_py_pb2",
    ],
)

py_test(
    name = "context_test",
    srcs = ["context_test.py"],
    deps = [":context"],
)
