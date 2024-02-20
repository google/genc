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
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/interop/backends/android:google_ai",
        "//generative_computing/cc/interop/backends/android:mediapipe_llm_inference",
        "//generative_computing/cc/interop/backends/android:open_ai",
        "//generative_computing/cc/intrinsics:handler_sets",
        "//generative_computing/cc/intrinsics:model_inference",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/cc/runtime:executor_stacks",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)
