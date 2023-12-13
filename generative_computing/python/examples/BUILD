load("@rules_python//python:defs.bzl", "py_binary", "py_test")

py_test(
    name = "authoring_demo",
    srcs = ["authoring_demo.py"],
    deps = ["//generative_computing"],
)

py_binary(
    name = "langchain_demo",
    srcs = ["langchain_demo.py"],
    deps = [
        "//generative_computing/python/interop",
        "//generative_computing/python/runtime",
    ],
)
