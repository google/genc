load("@com_github_grpc_grpc//bazel:python_rules.bzl", "py_proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

proto_library(
    name = "computation_proto",
    srcs = ["computation.proto"],
    deps = [
        "@com_google_protobuf//:any_proto",
    ],
)

cc_proto_library(
    name = "computation_cc_proto",
    deps = [":computation_proto"],
)

py_proto_library(
    name = "computation_py_pb2",
    deps = [":computation_proto"],
)

java_lite_proto_library(
    name = "computation_java_proto_lite",
    deps = [":computation_proto"],
)

proto_library(
    name = "executor_proto",
    srcs = ["executor.proto"],
    deps = [":computation_proto"],
)

cc_proto_library(
    name = "executor_cc_proto",
    deps = [":executor_proto"],
)

py_proto_library(
    name = "executor_py_pb2",
    deps = [":executor_proto"],
)

java_lite_proto_library(
    name = "executor_java_proto_lite",
    deps = [":executor_proto"],
)
