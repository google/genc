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
        "//genc/cc/interop/backends:llamacpp",
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

cc_library(
    name = "java_executor_stacks",
    srcs = ["java_executor_stacks.cc"],
    hdrs = ["java_executor_stacks.h"],
    deps = [
        "//genc/cc/authoring:constructor",
        "//genc/cc/interop/backends:llamacpp",
        "//genc/cc/interop/backends/java:google_ai",
        "//genc/cc/interop/backends/java:open_ai",
        "//genc/cc/interop/backends/java:wolfram_alpha_handler",
        "//genc/cc/intrinsics:handler_sets",
        "//genc/cc/intrinsics:model_inference",
        "//genc/cc/modules/agents:react",
        "//genc/cc/modules/retrieval:local_cache",
        "//genc/cc/runtime:executor",
        "//genc/cc/runtime:executor_stacks",
        "//genc/cc/runtime:status_macros",
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "//genc/proto/v0:computation_cc_proto",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)
