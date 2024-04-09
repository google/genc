package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_binary(
    name = "server",
    srcs = ["server.cc"],
    deps = [
        "//genc/cc/runtime:executor_service",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "//genc/proto/v0:executor_cc_grpc_proto",
        "//genc/proto/v0:executor_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)
