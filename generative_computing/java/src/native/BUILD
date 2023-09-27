# Java native interfaces for Generative computing components.

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

# TODO(b/295041820): Figure out how to make an android_jni_library instead.
exports_files(
    srcs = [
        "executor_jni.cc",
        "executor_jni.h",
        "owned_value_id_jni.cc",
        "owned_value_id_jni.h",
    ],
    visibility = ["//visibility:public"],
)
