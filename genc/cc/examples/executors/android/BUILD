load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "android_executor_stacks",
    srcs = ["android_executor_stacks.cc"],
    hdrs = ["android_executor_stacks.h"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/interop/backends:llamacpp",
        "//genc/cc/interop/backends/java:google_ai",
        "//genc/cc/interop/backends/java:mediapipe_llm_inference",
        "//genc/cc/interop/backends/java:open_ai",
        "//genc/cc/interop/backends/java:wolfram_alpha_handler",
        "//genc/cc/interop/networking:cronet_based_android_http_client",
        "//genc/cc/intrinsics:handler_sets",
        "//genc/cc/intrinsics:model_inference",
        "//genc/cc/modules/agents:react",
        "//genc/cc/modules/retrieval:local_cache",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:executor_stacks",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/proto/v0:computation_cc_proto",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)
