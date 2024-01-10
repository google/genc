load("@rules_python//python:defs.bzl", "py_binary", "py_test")

py_test(
    name = "authoring_demo",
    srcs = ["authoring_demo.py"],
    deps = ["//generative_computing"],
)

py_binary(
    name = "langchain_demo",
    srcs = ["langchain_demo.py"],
    # visibility = [
    #     "//generative_computing/python:python_packages",
    # ],
    deps = [
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/python/interop",
        "//generative_computing/python/runtime",
    ],
)

py_binary(
    name = "openai_demo",
    srcs = ["openai_demo.py"],
    visibility = [
        "//generative_computing/python:python_packages",
    ],
    deps = [
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/python/authoring",
        "//generative_computing/python/runtime",
        "@nlohmann_json//:json",
    ],
)
