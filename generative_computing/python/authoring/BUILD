load("@rules_python//python:defs.bzl", "py_library", "py_test")

package(
    default_visibility = ["//visibility:public"],
)

py_library(
    name = "authoring",
    srcs = ["__init__.py"],
    deps = [":constructors"],
)

py_library(
    name = "constructors",
    srcs = ["constructors.py"],
    data = [
        "//generative_computing/cc/authoring:constructor_bindings.so",
        "//generative_computing/cc/intrinsics:intrinsic_bindings.so",
        "//generative_computing/cc/runtime:executor_bindings.so",
    ],
    deps = ["//generative_computing/proto/v0:computation_py_pb2"],
)

py_test(
    name = "constructors_test",
    srcs = ["constructors_test.py"],
    deps = [":constructors"],
)
