load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

pybind_extension(
    name = "executor_bindings",
    srcs = ["executor_bindings.cc"],
    tags = ["generated_py_module=genc.examples.executors"],
    deps = [
        ":executor_stacks",
        "//genc/cc/runtime:executor",
        "//genc/proto/v0:computation_cc_proto",
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
        "//genc/cc/interop/backends:google_ai",
        "//genc/cc/intrinsics:handler_sets",
        "//genc/cc/modules/agents:react",
        "//genc/cc/modules/parsers:gemini_parser",
        "//genc/cc/modules/retrieval:local_cache",
        "//genc/cc/modules/tools:wolfram_alpha",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:executor_stacks",
        "//genc/cc/runtime:status_macros",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/status:statusor",
    ],
)
