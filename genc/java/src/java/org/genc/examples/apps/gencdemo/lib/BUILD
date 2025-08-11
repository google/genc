load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(
    default_visibility = ["//genc/java/src/java/org/genc/examples/apps/gencdemo:genc_demo_packages"],
)

licenses(["notice"])

cc_library(
    name = "android_executor_jni_lib",
    srcs = ["jni.cc"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//genc/c/runtime:c_api",
        "//genc/c/runtime:c_api_internal",
        "//genc/cc/examples/executors/android:android_executor_stacks",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "libandroid_executor_jni.so",
    linkopts = [
        "-Wl,--no-undefined",
    ],
    linkshared = 1,
    linkstatic = 1,
    deps = [":android_executor_jni_lib"],
)

cc_library(
    name = "libandroid_executor_jni_lib",
    srcs = [":libandroid_executor_jni.so"],
    alwayslink = 1,
)
