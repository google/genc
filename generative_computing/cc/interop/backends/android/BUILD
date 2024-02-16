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
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/intrinsics:intrinsic_uris",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/java/src/java/org/generativecomputing/interop/backends/openai/jni:open_ai_client_jni",
        "//generative_computing/proto/v0:computation_cc_proto",
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
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/intrinsics:intrinsic_uris",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/java/src/java/org/generativecomputing/interop/backends/googleai/jni:google_ai_client_jni",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@nlohmann_json//:json",
    ],
    alwayslink = 1,
)
