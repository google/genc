load("@rules_python//python:defs.bzl", "py_library", "py_test")

package_group(
    name = "gemini_interop",
    packages = [
        "//generative_computing/python/interop/gemini/...",
    ],
)

py_library(
    name = "gemini",
    srcs = ["__init__.py"],
    visibility = [
        "//generative_computing/python:python_packages",
    ],
    deps = [
        ":create_config",
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
