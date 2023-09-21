load("@rules_python//python:defs.bzl", "py_library")

package_group(
    name = "python_packages",
    packages = ["//generative_computing/..."],
)

py_library(
    name = "python",
    srcs = ["__init__.py"],
    visibility = [":python_packages"],
    deps = [
        "//generative_computing/python/authoring",
        "//generative_computing/python/interop",
        "//generative_computing/python/runtime",
    ],
)
