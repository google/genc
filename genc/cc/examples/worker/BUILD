package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_binary(
    name = "client",
    srcs = ["client.cc"],
    deps = [
        "//genc/cc/base:read_file",
        "//genc/cc/interop/confidential_computing:attestation",
        "//genc/cc/interop/oak:client",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:remote_executor",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:status_macros",
        "//genc/cc/runtime:threading",
        "//genc/proto/v0:executor_cc_grpc_proto",
        "//genc/proto/v0:executor_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_binary(
    name = "server",
    srcs = ["server.cc"],
    deps = [
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/modules/worker:run_server",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:status_macros",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)
