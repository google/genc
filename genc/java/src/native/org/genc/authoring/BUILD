load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

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
    linkopts = [
        "-Wl,--no-undefined",
    ],
    linkshared = 1,
    linkstatic = 1,
    deps = [
        ":constructor_jni_lib",
    ],
)

cc_library(
    name = "libconstructor_jni_lib",
    srcs = [":libconstructor_jni.so"],
    alwayslink = 1,
)
