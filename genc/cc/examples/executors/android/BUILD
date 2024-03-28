package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "android_executor_stacks",
    srcs = ["android_executor_stacks.cc"],
    hdrs = ["android_executor_stacks.h"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/interop/backends:llamacpp",
        "//genc/cc/interop/backends/android:google_ai",
        "//genc/cc/interop/backends/android:mediapipe_llm_inference",
        "//genc/cc/interop/backends/android:open_ai",
        "//genc/cc/interop/backends/android:wolfram_alpha_handler",
        "//genc/cc/intrinsics:handler_sets",
        "//genc/cc/intrinsics:model_inference",
        "//genc/cc/modules/agents:react",
        "//genc/cc/modules/retrieval:local_cache",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:executor_stacks",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)
