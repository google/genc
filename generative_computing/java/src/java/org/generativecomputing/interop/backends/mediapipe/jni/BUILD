package(
    default_visibility = ["//generative_computing:__subpackages__"],
)

licenses(["notice"])

cc_library(
    name = "llm_inference_client_jni",
    srcs = ["llm_inference_client_jni.cc"],
    hdrs = ["llm_inference_client_jni.h"],
    copts = [
        "-Wno-write-strings",  # The generated JNI header fails this.
        "-Wno-writable-strings",  # The generated JNI header fails this.
    ],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    deps = [
        "//generative_computing/java/src/jni:jni_utils",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
    ],
    alwayslink = 1,
)
