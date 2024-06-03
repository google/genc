load("@rules_python//python:defs.bzl", "py_library", "py_test")

package_group(
    name = "llamacpp_interop",
    packages = [
        "//genc/python/interop/llamacpp/...",
    ],
)

py_library(
    name = "llamacpp",
    srcs = ["__init__.py"],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        ":create_config",
        ":model_inference",
    ],
)

py_library(
    name = "create_config",
    srcs = ["create_config.py"],
)

py_test(
    name = "create_config_test",
    srcs = ["create_config_test.py"],
    deps = [":create_config"],
)

py_library(
    name = "model_inference",
    srcs = ["model_inference.py"],
    deps = [
        ":create_config",
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/base",
    ],
)

py_test(
    name = "model_inference_test",
    srcs = ["model_inference_test.py"],
    deps = [
        ":create_config",
        ":model_inference",
        "//genc/python/authoring",
    ],
)
