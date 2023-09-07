load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "authoring",
    srcs = ["__init__.py"],
    visibility = ["//generative_computing/python:python_packages"],
    deps = [":constructors"],
)

py_library(
    name = "constructors",
    srcs = ["constructors.py"],
    deps = [
        "//generative_computing/proto/v0:computation_py_pb2",
    ],
)
