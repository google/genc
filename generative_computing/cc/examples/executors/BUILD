load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

pybind_extension(
    name = "executor_bindings",
    srcs = ["executor_bindings.cc"],
    tags = ["generated_py_module=generative_computing.examples.executors"],
    deps = [
        ":executor_stacks",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/proto/v0:computation_cc_proto",
        "@pybind11_abseil//pybind11_abseil:absl_casters",
        "@pybind11_abseil//pybind11_abseil:status_casters",
        "@pybind11_protobuf//pybind11_protobuf:native_proto_caster",
    ],
)

cc_library(
    name = "executor_stacks",
    srcs = ["executor_stacks.cc"],
    hdrs = ["executor_stacks.h"],
    deps = [
        "//generative_computing/cc/interop/backends:google_ai",
        "//generative_computing/cc/intrinsics:handler_sets",
        "//generative_computing/cc/modules/agents:react",
        "//generative_computing/cc/modules/parsers:gemini_parser",
        "//generative_computing/cc/modules/retrieval:local_cache",
        "//generative_computing/cc/modules/tools:wolfram_alpha",
        "//generative_computing/cc/runtime:executor",
        "//generative_computing/cc/runtime:executor_stacks",
        "//generative_computing/cc/runtime:status_macros",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/status:statusor",
    ],
)
