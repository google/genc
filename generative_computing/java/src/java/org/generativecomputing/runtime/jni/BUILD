package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "executor_jni",
    srcs = ["executor_jni.cc"],
    hdrs = ["executor_jni.h"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    deps = [
        "//generative_computing/c/runtime:c_api",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/proto/v0:computation_cc_proto",
        "//generative_computing/proto/v0:executor_cc_proto",
    ],
    alwayslink = 1,
)

cc_library(
    name = "owned_value_id_jni",
    srcs = ["owned_value_id_jni.cc"],
    hdrs = ["owned_value_id_jni.h"],
    target_compatible_with = [
        "@platforms//os:android",
    ],
    deps = [
        "//generative_computing/c/runtime:c_api",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/proto/v0:computation_cc_proto",
        "//generative_computing/proto/v0:executor_cc_proto",
    ],
    alwayslink = 1,
)
