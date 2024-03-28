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
        "//genc/cc/authoring:constructor",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/jni:jni_utils",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)
