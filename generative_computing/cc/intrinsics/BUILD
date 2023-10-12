package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "intrinsics",
    srcs = ["intrinsics.cc"],
    hdrs = ["intrinsics.h"],
    deps = ["@com_google_absl//absl/strings"],
)

cc_library(
    name = "model_inference",
    srcs = ["model_inference.cc"],
    hdrs = ["model_inference.h"],
    deps = [
        ":intrinsics",
        "//generative_computing/cc/runtime:intrinsic_handler",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_github_google_re2//:re2",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "prompt_template",
    srcs = ["prompt_template.cc"],
    hdrs = ["prompt_template.h"],
    deps = [
        ":intrinsics",
        "//generative_computing/cc/runtime:intrinsic_handler",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_github_google_re2//:re2",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "regex_partial_match",
    srcs = ["regex_partial_match.cc"],
    hdrs = ["regex_partial_match.h"],
    deps = [
        ":intrinsics",
        "//generative_computing/cc/runtime:intrinsic_handler",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_github_google_re2//:re2",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)
