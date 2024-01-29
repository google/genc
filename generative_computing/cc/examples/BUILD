package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_binary(
    name = "run_openai",
    srcs = ["run_openai.cc"],
    deps = [
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/interop/backends:openai",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/cc/runtime:executor_stacks",
        "//generative_computing/cc/runtime:runner",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_binary(
    name = "run_llama_on_vertex",
    srcs = ["run_llama_on_vertex.cc"],
    deps = [
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/interop/backends:vertex_ai",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/cc/runtime:executor_stacks",
        "//generative_computing/cc/runtime:runner",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_binary(
    name = "run_gemini_on_vertex",
    srcs = ["run_gemini_on_vertex.cc"],
    deps = [
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/cc/runtime:executor_stacks",
        "//generative_computing/cc/runtime:runner",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)
