load("@fmeum_rules_jni//jni:defs.bzl", "cc_jni_library", "java_jni_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

java_jni_library(
    name = "default_executor",
    srcs = ["DefaultExecutor.java"],
    native_libs = [":default_executor_jni"],
)

cc_jni_library(
    name = "default_executor_jni",
    srcs = ["default_executor_jni.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":default_executor.hdrs",
        "//genc/c/runtime:c_api",
        "//genc/c/runtime:c_api_internal",
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/jni:jni_utils",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)
