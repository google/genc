# Libraries to access various LLM Backends. So user can stay backend agnostic.
package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "llamacpp",
    srcs = ["llamacpp.cc"],
    hdrs = ["llamacpp.h"],
    linkopts = [
        "-lm",
        "-ldl",
    ],
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
    alwayslink = 1,
)

cc_library(
    name = "google_ai",
    srcs = ["google_ai.cc"],
    hdrs = ["google_ai.h"],
    deps = [
        "//generative_computing/cc/intrinsics:model_inference_with_config",
        "//generative_computing/cc/modules/parsers:gemini_parser",
        "//generative_computing/cc/modules/tools:curl_client",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)
