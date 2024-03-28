package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "open_ai",
    srcs = ["open_ai.cc"],
    hdrs = ["open_ai.h"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/intrinsics:intrinsic_uris",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/java/org/genc/interop/backends/openai/jni:open_ai_client_jni",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@nlohmann_json//:json",
    ],
    alwayslink = 1,
)

cc_library(
    name = "google_ai",
    srcs = ["google_ai.cc"],
    hdrs = ["google_ai.h"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/intrinsics:intrinsic_uris",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/java/org/genc/interop/backends/googleai/jni:google_ai_client_jni",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@nlohmann_json//:json",
    ],
    alwayslink = 1,
)

cc_library(
    name = "mediapipe_llm_inference",
    srcs = ["mediapipe_llm_inference.cc"],
    hdrs = ["mediapipe_llm_inference.h"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/intrinsics:intrinsic_uris",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/java/org/genc/interop/backends/mediapipe/jni:llm_inference_client_jni",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "wolfram_alpha_handler",
    srcs = ["wolfram_alpha_handler.cc"],
    hdrs = ["wolfram_alpha_handler.h"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    deps = [
        "//genc/cc/intrinsics:intrinsic_uris",
        "//genc/cc/runtime:intrinsic_handler",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/java/org/genc/interop/backends/wolframalpha/jni:wolfram_alpha_client_jni",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)
