load("@rules_python//python:defs.bzl", "py_library", "py_test")

py_library(
    name = "langchain",
    srcs = ["__init__.py"],
    visibility = ["//generative_computing/python:python_packages"],
    deps = [
        ":create_computation",
        ":custom_model",
        ":model_cascade",
    ],
)

py_library(
    name = "create_computation",
    srcs = ["create_computation.py"],
    deps = [
        ":custom_model",
        ":model_cascade",
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/python/authoring",
    ],
)

py_test(
    name = "create_computation_test",
    srcs = ["create_computation_test.py"],
    deps = [
        ":create_computation",
        ":custom_model",
        ":model_cascade",
        "//generative_computing/python/authoring",
    ],
)

py_library(
    name = "custom_model",
    srcs = ["custom_model.py"],
)

py_test(
    name = "custom_model_test",
    srcs = ["custom_model_test.py"],
    deps = [":custom_model"],
)

py_library(
    name = "model_cascade",
    srcs = ["model_cascade.py"],
)

py_test(
    name = "model_cascade_test",
    srcs = ["model_cascade_test.py"],
    deps = [
        ":custom_model",
        ":model_cascade",
    ],
)
