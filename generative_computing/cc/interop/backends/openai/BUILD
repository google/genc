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

cc_library(
    name = "openai_executor_stacks",
    srcs = ["openai_executor_stacks.cc"],
    hdrs = ["openai_executor_stacks.h"],
    deps = [
        ":openai",
        "//generative_computing/cc/intrinsics:handler_sets",
        "//generative_computing/cc/modules/agents:react",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/cc/runtime:executor_stacks",
        "//generative_computing/cc/runtime:status_macros",
        "@com_google_absl//absl/status:statusor",
    ],
)
