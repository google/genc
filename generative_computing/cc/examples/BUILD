package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_binary(
    name = "run_openai",
    srcs = ["run_openai.cc"],
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
    ],
)

cc_binary(
    name = "run_llama_on_vertex",
    srcs = ["run_llama_on_vertex.cc"],
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

cc_binary(
    name = "run_local_model_with_llamacpp",
    srcs = ["run_local_model_with_llamacpp.cc"],
    deps = [
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/interop/backends:llamacpp",
        "//generative_computing/cc/intrinsics:handler_sets",
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
    name = "run_gemini_on_ai_studio",
    srcs = ["run_gemini_on_ai_studio.cc"],
    deps = [
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/authoring:smart_chain",
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
    name = "run_wolfram",
    srcs = ["run_wolfram.cc"],
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
    ],
)

cc_binary(
    name = "math_tool_agent",
    srcs = ["math_tool_agent.cc"],
    deps = [
        "//generative_computing/cc/authoring:constructor",
        "//generative_computing/cc/authoring:smart_chain",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/cc/runtime:executor_stacks",
        "//generative_computing/cc/runtime:runner",
        "//generative_computing/cc/runtime:status_macros",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:string_view",
    ],
)
