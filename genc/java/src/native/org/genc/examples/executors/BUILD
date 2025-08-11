load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "default_executor_jni_lib",
    srcs = ["default_executor.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//genc/c/runtime:c_api",
        "//genc/c/runtime:c_api_internal",
        "//genc/cc/examples/executors:java_executor_stacks",
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
    name = "libdefault_executor_jni.so",
    linkopts = [
        "-Wl,--no-undefined",
    ],
    linkshared = 1,
    linkstatic = 1,
    deps = [":default_executor_jni_lib"],
)

cc_library(
    name = "libdefault_executor_jni_lib",
    srcs = [":libdefault_executor_jni.so"],
    visibility = ["//visibility:public"],
    alwayslink = 1,
)
