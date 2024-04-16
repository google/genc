package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "read_file",
    srcs = ["read_file.cc"],
    hdrs = ["read_file.h"],
    deps = ["@com_google_absl//absl/strings"],
)

cc_library(
    name = "to_from_grpc_status",
    srcs = ["to_from_grpc_status.cc"],
    hdrs = ["to_from_grpc_status.h"],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)
