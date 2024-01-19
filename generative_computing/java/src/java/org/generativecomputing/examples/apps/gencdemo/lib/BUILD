package(
    default_visibility = ["//generative_computing/java/src/java/org/generativecomputing/examples/apps/gencdemo:genc_demo_packages"],
)

licenses(["notice"])

cc_library(
    name = "jni",
    srcs = ["jni.cc"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    visibility = ["//generative_computing/java/src/java/org/generativecomputing/examples/apps/gencdemo:genc_demo_packages"],
    deps = [
        "//generative_computing/c/runtime:c_api",
        "//generative_computing/c/runtime:c_api_internal",
        "//generative_computing/cc/runtime/android:android_executor_stacks",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)
