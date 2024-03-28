# Java native interfaces for GenC components.

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

exports_files(
    srcs = [
        "executor_jni.cc",
        "executor_jni.h",
        "owned_value_id_jni.cc",
        "owned_value_id_jni.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jni_utils",
    srcs = ["jni_utils.cc"],
    hdrs = ["jni_utils.h"],
    copts = [
        "-Wno-write-strings",  # The generated JNI header fails this.
        "-Wno-writable-strings",  # The generated JNI header fails this.
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)
