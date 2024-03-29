package(
    default_visibility = ["//genc/java/src/java/org/genc/examples:__subpackages__"],
)

licenses(["notice"])

cc_library(
    name = "jni",
    srcs = ["jni.cc"],
    visibility = ["//genc/java/src/java/org/genc/examples/executors:__subpackages__"],
    deps = [
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
    alwayslink = 1,
)
