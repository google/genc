load("@rules_python//python:defs.bzl", "py_library")

# Package generative_computing

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

exports_files(["LICENSE"])

py_library(
    name = "generative_computing",
    srcs = ["__init__.py"],
    visibility = ["//visibility:public"],
    deps = [
        "//generative_computing/python/authoring",
        "//generative_computing/python/examples",
        "//generative_computing/python/interop",
        "//generative_computing/python/runtime",
    ],
)
