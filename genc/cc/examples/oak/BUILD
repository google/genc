package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_binary(
    name = "client",
    srcs = ["client.cc"],
    deps = [
        "//genc/cc/base:read_file",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "//genc/proto/v0:executor_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@oak//cc/attestation/verification:attestation_verifier",
        "@oak//cc/client",
        "@oak//cc/transport",
        "@oak//cc/transport:grpc_unary_transport",
        "@oak//proto/session:service_unary_cc_grpc",
        "@oak//proto/session:service_unary_cc_proto",
    ],
)
