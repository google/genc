load("@rules_python//python:defs.bzl", "py_library", "py_test")

package_group(
    name = "langchain_interop",
    packages = [
        "//generative_computing/python/interop/langchain/...",
    ],
)

py_library(
    name = "langchain",
    srcs = ["__init__.py"],
    visibility = [
        "//generative_computing/python:python_packages",
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
        "//generative_computing/python/authoring",
        "//generative_computing/python/base:to_from_value_proto",
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
        "//generative_computing/python/authoring",
        "//generative_computing/python/base:to_from_value_proto",
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
    visibility = [
        "//visibility:private",  # Only private by automation, not intent. Owner may accept CLs adding visibility. See go/scheuklappen#explicit-private.
    ],
    deps = ["//generative_computing/proto/v0:computation_py_pb2"],
)

py_test(
    name = "custom_chain_test",
    srcs = ["custom_chain_test.py"],
    deps = [":custom_chain"],
)

py_library(
    name = "custom_tool",
    srcs = ["custom_tool.py"],
    visibility = [
        "//visibility:private",  # Only private by automation, not intent. Owner may accept CLs adding visibility. See go/scheuklappen#explicit-private.
    ],
    deps = [
        "//generative_computing/proto/v0:computation_py_pb2",
        "//generative_computing/python/interop/langchain:custom_chain",
    ],
)

py_test(
    name = "custom_tool_test",
    srcs = ["custom_tool_test.py"],
    deps = [
        ":custom_tool",
        "//generative_computing/python/interop/langchain:custom_chain",
    ],
)

py_library(
    name = "custom_agent",
    srcs = ["custom_agent.py"],
    visibility = [
        "//visibility:private",  # Only private by automation, not intent. Owner may accept CLs adding visibility. See go/scheuklappen#explicit-private.
    ],
    deps = [":custom_tool"],
)

py_test(
    name = "custom_agent_test",
    srcs = ["custom_agent_test.py"],
    deps = [
        ":custom_agent",
        "//generative_computing/python/authoring",
        "//generative_computing/python/interop/langchain:custom_chain",
        "//generative_computing/python/interop/langchain:custom_model",
        "//generative_computing/python/interop/langchain:custom_tool",
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
