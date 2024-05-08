package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_binary(
    name = "run_openai",
    srcs = ["run_openai.cc"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_binary(
    name = "run_llama_on_vertex",
    srcs = ["run_llama_on_vertex.cc"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_binary(
    name = "run_gemini_on_vertex",
    srcs = ["run_gemini_on_vertex.cc"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
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
        "//genc/cc/authoring:constructor",
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/interop/backends:llamacpp",
        "//genc/cc/intrinsics:handler_sets",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:executor_stacks",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_binary(
    name = "run_gemini_on_ai_studio",
    srcs = ["run_gemini_on_ai_studio.cc"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/authoring:smart_chain",
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_binary(
    name = "run_wolfram",
    srcs = ["run_wolfram.cc"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_binary(
    name = "math_tool_agent",
    srcs = ["math_tool_agent.cc"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/authoring:smart_chain",
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_binary(
    name = "custom_concurrency_interface",
    srcs = ["custom_concurrency_interface.cc"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/examples/executors:executor_stacks",
        "//genc/cc/runtime:concurrency",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "multivariate_prompt_template_test",
    srcs = ["multivariate_prompt_template_test.cc"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/runtime:executor_stacks",
        "//genc/cc/runtime:runner",
        "//genc/cc/runtime:status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)
