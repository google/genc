# Libraries to access various LLM Backends. So user can stay backend agnostic.
package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "llamacpp",
    srcs = ["llamacpp.cc"],
    hdrs = ["llamacpp.h"],
    deps = [
        "//generative_computing/cc/intrinsics:model_inference",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@llama_cpp",
    ],
)
