load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "run_server",
    srcs = ["run_server.cc"],
    hdrs = ["run_server.h"],
    deps = [
        "//genc/cc/base:read_file",
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/interop/confidential_computing:attestation",
        "//genc/cc/interop/oak:server",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:executor_service",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "//genc/proto/v0:executor_cc_grpc_proto",
        "//genc/proto/v0:executor_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@oak//proto/session:service_unary_cc_grpc",
    ],
)
