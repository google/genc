load("@rules_python//python:defs.bzl", "py_library", "py_test")

py_library(
    name = "authoring",
    srcs = ["__init__.py"],
    visibility = ["//genc/python:python_packages"],
    deps = [
        ":constructors",
        ":tracing_decorator",
        ":tracing_intrinsics",
    ],
)

py_library(
    name = "computation_from_intrinsic",
    srcs = ["computation_from_intrinsic.py"],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/base",
    ],
)

py_library(
    name = "constructors",
    srcs = ["constructors.py"],
    data = [
        "//genc/cc/authoring:constructor_bindings.so",
        "//genc/cc/intrinsics:intrinsic_bindings.so",
        "//genc/cc/runtime:executor_bindings.so",
    ],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/base:to_from_value_proto",
    ],
)

py_test(
    name = "constructors_test",
    srcs = ["constructors_test.py"],
    deps = [
        ":constructors",
        "//genc/python/base:to_from_value_proto",
    ],
)

py_library(
    name = "tracing_context",
    srcs = ["tracing_context.py"],
    data = [
        "//genc/cc/authoring:constructor_bindings.so",
        "//genc/cc/intrinsics:intrinsic_bindings.so",
        "//genc/cc/runtime:executor_bindings.so",
    ],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/base",
    ],
)

py_library(
    name = "tracing_decorator",
    srcs = ["tracing_decorator.py"],
    data = [
        "//genc/cc/authoring:constructor_bindings.so",
        "//genc/cc/intrinsics:intrinsic_bindings.so",
        "//genc/cc/runtime:executor_bindings.so",
    ],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        ":tracing_context",
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/base",
    ],
)

py_library(
    name = "tracing_intrinsics",
    srcs = ["tracing_intrinsics.py"],
    data = [
        "//genc/cc/authoring:constructor_bindings.so",
        "//genc/cc/intrinsics:intrinsic_bindings.so",
        "//genc/cc/runtime:executor_bindings.so",
    ],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        ":computation_from_intrinsic",
        "//genc/python/base",
    ],
)

py_test(
    name = "tracing_test",
    srcs = ["tracing_test.py"],
    deps = [
        ":constructors",
        ":tracing_context",
        ":tracing_decorator",
        ":tracing_intrinsics",
        "//genc/python/runtime",
    ],
)
