package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "constructor_jni_lib",
    srcs = ["constructor.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/java/src/jni:jni_utils",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "libconstructor_jni.so",
    linkshared = True,
    deps = [":constructor_jni_lib"],
)
