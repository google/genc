package(
    default_visibility = ["//genc:__subpackages__"],
)

licenses(["notice"])

cc_library(
    name = "google_ai_client_jni",
    srcs = ["google_ai_client_jni.cc"],
    hdrs = ["google_ai_client_jni.h"],
    copts = [
        "-Wno-write-strings",  # The generated JNI header fails this.
        "-Wno-writable-strings",  # The generated JNI header fails this.
    ],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    deps = [
        "//genc/java/src/jni:jni_utils",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
    ],
    alwayslink = 1,
)
