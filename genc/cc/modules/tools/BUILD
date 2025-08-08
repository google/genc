load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "wolfram_alpha",
    srcs = ["wolfram_alpha.cc"],
    hdrs = ["wolfram_alpha.h"],
    deps = [
        "//genc/cc/intrinsics:intrinsic_uris",
        "//genc/cc/runtime:intrinsic_handler",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
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
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@curl",
    ],
)
