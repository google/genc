load("@rules_python//python:defs.bzl", "py_library")

package_group(
    name = "python_packages",
    packages = [
        "//genc/...",
    ],
)

py_library(
    name = "python",
    srcs = ["__init__.py"],
    visibility = ["//visibility:private"],
    deps = [
        "//genc/python/authoring",
        "//genc/python/examples",
        "//genc/python/interop",
        "//genc/python/runtime",
    ],
)
