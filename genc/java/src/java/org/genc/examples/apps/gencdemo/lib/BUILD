package(
    default_visibility = ["//genc/java/src/java/org/genc/examples/apps/gencdemo:genc_demo_packages"],
)

licenses(["notice"])

cc_library(
    name = "jni",
    srcs = ["jni.cc"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    visibility = ["//genc/java/src/java/org/genc/examples/apps/gencdemo:genc_demo_packages"],
    deps = [
        "//genc/c/runtime:c_api",
        "//genc/c/runtime:c_api_internal",
        "//genc/cc/examples/executors/android:android_executor_stacks",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)
