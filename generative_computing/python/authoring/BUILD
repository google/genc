load("@rules_python//python:defs.bzl", "py_library", "py_test")

py_library(
    name = "authoring",
    srcs = ["__init__.py"],
    visibility = ["//generative_computing/python:python_packages"],
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
        "//generative_computing/python:python_packages",
    ],
    deps = [
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/python/base",
    ],
)

py_library(
    name = "constructors",
    srcs = ["constructors.py"],
    data = [
        "//generative_computing/cc/authoring:constructor_bindings.so",
        "//generative_computing/cc/intrinsics:intrinsic_bindings.so",
        "//generative_computing/cc/runtime:executor_bindings.so",
    ],
    visibility = [
        "//generative_computing/python:python_packages",
    ],
    deps = ["//generative_computing/proto/v0:computation_py_pb2"],
)

py_test(
    name = "constructors_test",
    srcs = ["constructors_test.py"],
    deps = [":constructors"],
)

py_library(
    name = "tracing_context",
    srcs = ["tracing_context.py"],
    data = [
        "//generative_computing/cc/authoring:constructor_bindings.so",
        "//generative_computing/cc/intrinsics:intrinsic_bindings.so",
        "//generative_computing/cc/runtime:executor_bindings.so",
    ],
    visibility = [
        "//generative_computing/python:python_packages",
    ],
    deps = [
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/python/base",
    ],
)

py_library(
    name = "tracing_decorator",
    srcs = ["tracing_decorator.py"],
    data = [
        "//generative_computing/cc/authoring:constructor_bindings.so",
        "//generative_computing/cc/intrinsics:intrinsic_bindings.so",
        "//generative_computing/cc/runtime:executor_bindings.so",
    ],
    visibility = [
        "//generative_computing/python:python_packages",
    ],
    deps = [
        ":tracing_context",
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/python/base",
    ],
)

py_library(
    name = "tracing_intrinsics",
    srcs = ["tracing_intrinsics.py"],
    data = [
        "//generative_computing/cc/authoring:constructor_bindings.so",
        "//generative_computing/cc/intrinsics:intrinsic_bindings.so",
        "//generative_computing/cc/runtime:executor_bindings.so",
    ],
    visibility = [
        "//generative_computing/python:python_packages",
    ],
    deps = [
        ":computation_from_intrinsic",
        "//generative_computing/python/base",
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
        "//generative_computing/python/runtime",
    ],
)
