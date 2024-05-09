package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "open_ai",
    srcs = ["open_ai.cc"],
    hdrs = ["open_ai.h"],
    deps = [
        ":open_ai_client_jni",
        "//genc/cc/authoring:constructor",
        "//genc/cc/intrinsics:intrinsic_uris",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/proto/v0:computation_cc_proto",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@nlohmann_json//:json",
    ],
    alwayslink = 1,
)

cc_library(
    name = "open_ai_client_jni",
    srcs = ["open_ai_client_jni.cc"],
    hdrs = ["open_ai_client_jni.h"],
    copts = [
        "-Wno-write-strings",  # The generated JNI header fails this.
        "-Wno-writable-strings",  # The generated JNI header fails this.
    ],
    deps = [
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/java/src/jni:jni_utils",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
    ],
    alwayslink = 1,
)

cc_library(
    name = "google_ai",
    srcs = ["google_ai.cc"],
    hdrs = ["google_ai.h"],
    deps = [
        ":google_ai_client_jni",
        "//genc/cc/authoring:constructor",
        "//genc/cc/intrinsics:intrinsic_uris",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/proto/v0:computation_cc_proto",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@nlohmann_json//:json",
    ],
    alwayslink = 1,
)

cc_library(
    name = "google_ai_client_jni",
    srcs = ["google_ai_client_jni.cc"],
    hdrs = ["google_ai_client_jni.h"],
    copts = [
        "-Wno-write-strings",  # The generated JNI header fails this.
        "-Wno-writable-strings",  # The generated JNI header fails this.
    ],
    deps = [
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/java/src/jni:jni_utils",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
    ],
    alwayslink = 1,
)

cc_library(
    name = "mediapipe_llm_inference",
    srcs = ["mediapipe_llm_inference.cc"],
    hdrs = ["mediapipe_llm_inference.h"],
    deps = [
        ":mediapipe_llm_inference_client_jni",
        "//genc/cc/authoring:constructor",
        "//genc/cc/intrinsics:intrinsic_uris",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/proto/v0:computation_cc_proto",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "mediapipe_llm_inference_client_jni",
    srcs = ["mediapipe_llm_inference_client_jni.cc"],
    hdrs = ["mediapipe_llm_inference_client_jni.h"],
    copts = [
        "-Wno-write-strings",  # The generated JNI header fails this.
        "-Wno-writable-strings",  # The generated JNI header fails this.
    ],
    deps = [
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/java/src/jni:jni_utils",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
    ],
    alwayslink = 1,
)

cc_library(
    name = "wolfram_alpha_handler",
    srcs = ["wolfram_alpha_handler.cc"],
    hdrs = ["wolfram_alpha_handler.h"],
    deps = [
        ":wolfram_alpha_client_jni",
        "//genc/cc/intrinsics:intrinsic_uris",
        "//genc/cc/runtime:intrinsic_handler",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/proto/v0:computation_cc_proto",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "wolfram_alpha_client_jni",
    srcs = ["wolfram_alpha_client_jni.cc"],
    hdrs = ["wolfram_alpha_client_jni.h"],
    copts = [
        "-Wno-write-strings",  # The generated JNI header fails this.
        "-Wno-writable-strings",  # The generated JNI header fails this.
    ],
    deps = [
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/java/src/jni:jni_utils",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
    ],
    alwayslink = 1,
)
