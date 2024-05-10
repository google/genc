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

cc_library(
    name = "computation",
    srcs = ["computation.cc"],
    hdrs = ["computation.h"],
    deps = [
        ":context",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "context",
    srcs = ["context.cc"],
    hdrs = ["context.h"],
    deps = [
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "context_test",
    srcs = ["context_test.cc"],
    deps = [
        ":context",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "computation_test",
    srcs = ["computation_test.cc"],
    deps = [
        ":computation",
        ":context",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)
