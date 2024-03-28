load("@rules_python//python:defs.bzl", "py_binary", "py_library", "py_test")

py_library(
    name = "examples",
    srcs = ["__init__.py"],
    visibility = ["//genc/python:python_packages"],
    deps = [
        ":executor",
    ],
)

py_library(
    name = "executor",
    srcs = ["executor.py"],
    data = [
        "//genc/cc/examples/executors:executor_bindings.so",
        "//genc/cc/runtime:executor_bindings.so",
    ],
    visibility = [
        "//genc/python:python_packages",
    ],
)

py_test(
    name = "authoring_demo",
    srcs = ["authoring_demo.py"],
    deps = ["//genc"],
)

py_binary(
    name = "langchain_demo",
    srcs = ["langchain_demo.py"],
    deps = [
        ":executor",
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/interop",
        "//genc/python/runtime",
    ],
)

py_binary(
    name = "langchain_agent_demo",
    srcs = ["langchain_agent_demo.py"],
    deps = [
        ":executor",
        "//genc",
    ],
)

py_binary(
    name = "openai_demo",
    srcs = ["openai_demo.py"],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        ":executor",
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/authoring",
        "//genc/python/runtime",
    ],
)

py_binary(
    name = "tools_wolfram_demo",
    srcs = ["tools_wolfram_demo.py"],
    deps = [
        ":executor",
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/authoring",
        "//genc/python/runtime",
    ],
)

py_binary(
    name = "math_tool_agent",
    srcs = ["math_tool_agent.py"],
    deps = [
        ":executor",
        "//genc",
    ],
)
