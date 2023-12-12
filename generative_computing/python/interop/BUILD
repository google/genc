load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "interop",
    srcs = ["__init__.py"],
    visibility = [
        "//generative_computing:__subpackages__",
        "//shopping/serving/ai/lm_server/shared/genc_provider:__subpackages__",
    ],
    deps = ["//generative_computing/python/interop/langchain"],
)
