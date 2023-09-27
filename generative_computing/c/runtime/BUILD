# C API for Generative computing.

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "c_api",
    srcs = ["c_api.cc"],
    hdrs = [
        "c_api.h",
    ],
    deps = [
        ":c_api_internal",
        ":c_api_macros_hdrs",
        ":gc_buffer",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/cc/runtime:executor_stacks",
        "//generative_computing/proto/v0:executor_cc_proto",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "c_api_macros_hdrs",
    hdrs = [
        "c_api_macros.h",
    ],
)

cc_library(
    name = "gc_buffer",
    srcs = ["gc_buffer.cc"],
    hdrs = [
        "gc_buffer.h",
    ],
    deps = [":c_api_macros_hdrs"],
)

cc_library(
    name = "c_api_internal",
    hdrs = [
        "c_api_internal.h",
    ],
    deps = [
        "//generative_computing/cc/runtime:executor",
    ],
)
