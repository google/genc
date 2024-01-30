# Libraries to access various LLM Backends. So user can stay backend agnostic.
package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "openai",
    srcs = ["openai.cc"],
    hdrs = ["openai.h"],
    deps = [
        "//generative_computing/cc/intrinsics:model_inference",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@curl",
    ],
)
