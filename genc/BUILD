load("@rules_python//python:defs.bzl", "py_library")

# Package genc

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

exports_files(["LICENSE"])

py_library(
    name = "genc",
    srcs = ["__init__.py"],
    visibility = ["//visibility:public"],
    deps = [
        "//genc/python/authoring",
        "//genc/python/examples",
        "//genc/python/interop",
        "//genc/python/runtime",
    ],
)
