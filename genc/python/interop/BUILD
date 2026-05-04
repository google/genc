load("@rules_python//python:defs.bzl", "py_library")

licenses(["notice"])

py_library(
    name = "interop",
    srcs = ["__init__.py"],
    visibility = [
        "//genc:__subpackages__",
    ],
    deps = [
        "//genc/python/interop/gemini",
        "//genc/python/interop/langchain",
        "//genc/python/interop/llamacpp",
    ],
)
