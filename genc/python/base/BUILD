load("@rules_python//python:defs.bzl", "py_library", "py_test")

py_library(
    name = "base",
    srcs = ["__init__.py"],
    visibility = ["//genc/python:python_packages"],
    deps = [
        ":computation",
        ":context",
        ":embed",
        ":to_from_value_proto",
    ],
)

py_library(
    name = "computation",
    srcs = ["computation.py"],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        ":context",
        "//genc/proto/v0:computation_py_pb2",
    ],
)

py_test(
    name = "computation_test",
    srcs = ["computation_test.py"],
    deps = [
        ":computation",
        ":context",
        "//genc/proto/v0:computation_py_pb2",
    ],
)

py_library(
    name = "context",
    srcs = ["context.py"],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        "//genc/proto/v0:computation_py_pb2",
    ],
)

py_test(
    name = "context_test",
    srcs = ["context_test.py"],
    deps = [":context"],
)

py_library(
    name = "embed",
    srcs = ["embed.py"],
    visibility = ["//genc/python:python_packages"],
    deps = [":context"],
)

py_test(
    name = "embed_test",
    srcs = ["embed_test.py"],
    deps = [
        ":embed",
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/authoring:constructors",
        "//genc/python/authoring:tracing_decorator",
    ],
)

py_library(
    name = "to_from_value_proto",
    srcs = ["to_from_value_proto.py"],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        ":computation",
        "//genc/proto/v0:computation_py_pb2",
    ],
)

py_test(
    name = "to_from_value_proto_test",
    srcs = ["to_from_value_proto_test.py"],
    deps = [
        ":to_from_value_proto",
        "//genc/proto/v0:computation_py_pb2",
    ],
)
