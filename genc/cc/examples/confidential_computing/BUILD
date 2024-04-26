package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_binary(
    name = "client",
    srcs = ["client.cc"],
    deps = [
        "//genc/cc/runtime:status_macros",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
        "@oak//proto/session:service_unary_cc_grpc",
    ],
)

cc_binary(
    name = "debug_attestation",
    srcs = ["debug_attestation.cc"],
    deps = [
        "//genc/cc/interop/confidential_computing:attestation",
        "//genc/cc/runtime:status_macros",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@tink_cc//tink/jwt:verified_jwt",
    ],
)
