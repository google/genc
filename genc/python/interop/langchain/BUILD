load("@rules_python//python:defs.bzl", "py_library", "py_test")

package_group(
    name = "langchain_interop",
    packages = [
        "//genc/python/interop/langchain/...",
    ],
)

py_library(
    name = "langchain",
    srcs = ["__init__.py"],
    visibility = [
        "//genc/python:python_packages",
    ],
    deps = [
        ":create_computation",
        ":custom_agent",
        ":custom_chain",
        ":custom_model",
        ":custom_tool",
        ":model_cascade",
    ],
)

py_library(
    name = "create_computation",
    srcs = ["create_computation.py"],
    deps = [
        ":custom_chain",
        ":custom_model",
        ":model_cascade",
        "//genc/python/authoring",
        "//genc/python/base:to_from_value_proto",
    ],
)

py_test(
    name = "create_computation_test",
    srcs = ["create_computation_test.py"],
    deps = [
        ":create_computation",
        ":custom_chain",
        ":custom_model",
        ":model_cascade",
        "//genc/python/authoring",
        "//genc/python/base:to_from_value_proto",
    ],
)

py_library(
    name = "custom_model",
    srcs = ["custom_model.py"],
)

py_test(
    name = "custom_model_test",
    srcs = ["custom_model_test.py"],
    deps = [":custom_model"],
)

py_library(
    name = "custom_chain",
    srcs = ["custom_chain.py"],
    visibility = ["//visibility:private"],
    deps = ["//genc/proto/v0:computation_py_pb2"],
)

py_test(
    name = "custom_chain_test",
    srcs = ["custom_chain_test.py"],
    deps = [":custom_chain"],
)

py_library(
    name = "custom_tool",
    srcs = ["custom_tool.py"],
    visibility = ["//visibility:private"],
    deps = [
        "//genc/proto/v0:computation_py_pb2",
        "//genc/python/interop/langchain:custom_chain",
    ],
)

py_test(
    name = "custom_tool_test",
    srcs = ["custom_tool_test.py"],
    deps = [
        ":custom_tool",
        "//genc/python/interop/langchain:custom_chain",
    ],
)

py_library(
    name = "custom_agent",
    srcs = ["custom_agent.py"],
    visibility = ["//visibility:private"],
    deps = [":custom_tool"],
)

py_test(
    name = "custom_agent_test",
    srcs = ["custom_agent_test.py"],
    deps = [
        ":custom_agent",
        "//genc/python/authoring",
        "//genc/python/interop/langchain:custom_chain",
        "//genc/python/interop/langchain:custom_model",
        "//genc/python/interop/langchain:custom_tool",
    ],
)

py_library(
    name = "model_cascade",
    srcs = ["model_cascade.py"],
)

py_test(
    name = "model_cascade_test",
    srcs = ["model_cascade_test.py"],
    deps = [
        ":custom_model",
        ":model_cascade",
    ],
)
