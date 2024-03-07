package(
    default_visibility = ["//generative_computing:__subpackages__"],
)

licenses(["notice"])

cc_library(
    name = "jni_utils",
    srcs = ["jni_utils.cc"],
    hdrs = ["jni_utils.h"],
    copts = [
        "-Wno-write-strings",  # The generated JNI header fails this.
        "-Wno-writable-strings",  # The generated JNI header fails this.
    ],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    alwayslink = 1,
)
