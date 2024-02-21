package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "wolfram_alpha",
    srcs = ["wolfram_alpha.cc"],
    hdrs = ["wolfram_alpha.h"],
    deps = [
        "//generative_computing/cc/intrinsics:intrinsic_uris",
        "//generative_computing/cc/runtime:intrinsic_handler",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@curl",
    ],
)

cc_library(
    name = "curl_client",
    srcs = ["curl_client.cc"],
    hdrs = ["curl_client.h"],
    deps = [
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@curl",
    ],
)
