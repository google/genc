package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "constructor_jni",
    srcs = ["constructor_jni.cc"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/java/src/jni:jni_utils",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)
