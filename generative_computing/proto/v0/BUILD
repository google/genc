load("@com_github_grpc_grpc//bazel:python_rules.bzl", "py_proto_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

proto_library(
    name = "computation_proto",
    srcs = ["computation.proto"],
)

py_proto_library(
    name = "computation_py_pb2",
    api_version = 2,
    deps = [":computation_proto"],
)
