load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "control_flow_executor",
    srcs = ["control_flow_executor.cc"],
    hdrs = ["control_flow_executor.h"],
    deps = [
        ":executor",
        ":intrinsic_handler",
        ":status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "control_flow_executor_test",
    timeout = "short",
    srcs = ["control_flow_executor_test.cc"],
    deps = [
        ":control_flow_executor",
        ":executor",
        ":inline_executor",
        ":intrinsic_handler",
        ":runner",
        ":status_macros",
        "//genc/cc/authoring:constructor",
        "//genc/cc/intrinsics:custom_function",
        "//genc/cc/intrinsics:handler_sets",
        "//genc/cc/intrinsics:model_inference",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/types:span",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "executor",
    srcs = ["executor.cc"],
    hdrs = ["executor.h"],
    deps = [
        ":status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "//genc/proto/v0:executor_cc_proto",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "executor_test",
    timeout = "short",
    srcs = ["executor_test.cc"],
    deps = [
        ":executor",
        "//genc/proto/v0:executor_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@com_google_googletest//:gtest_main",
    ],
)

pybind_extension(
    name = "executor_bindings",
    srcs = ["executor_bindings.cc"],
    tags = ["generated_py_module=genc.runtime"],
    deps = [
        ":executor",
        ":executor_stacks",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
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
        ":control_flow_executor",
        ":executor",
        ":inline_executor",
        ":intrinsic_handler",
        ":status_macros",
        "//genc/cc/interop/backends:google_ai",
        "//genc/cc/intrinsics:handler_sets",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "executor_stacks_test",
    timeout = "short",
    srcs = ["executor_stacks_test.cc"],
    deps = [
        ":executor",
        ":executor_stacks",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "inline_executor",
    srcs = ["inline_executor.cc"],
    hdrs = ["inline_executor.h"],
    deps = [
        ":executor",
        ":intrinsic_handler",
        ":status_macros",
        ":threading",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "inline_executor_test",
    timeout = "short",
    srcs = ["inline_executor_test.cc"],
    deps = [
        ":executor",
        ":inline_executor",
        ":runner",
        "//genc/cc/authoring:constructor",
        "//genc/cc/intrinsics:handler_sets",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "intrinsic_handler",
    srcs = ["intrinsic_handler.cc"],
    hdrs = ["intrinsic_handler.h"],
    deps = [
        ":executor",
        ":status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "status_macros",
    hdrs = ["status_macros.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "threading",
    srcs = ["threading.cc"],
    hdrs = ["threading.h"],
    deps = [
        ":status_macros",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
    ],
)

cc_library(
    name = "runner",
    srcs = ["runner.cc"],
    hdrs = ["runner.h"],
    deps = [
        ":executor",
        ":executor_stacks",
        ":status_macros",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "runner_test",
    srcs = ["runner_test.cc"],
    deps = [
        ":executor",
        ":executor_stacks",
        ":runner",
        "//genc/cc/authoring:constructor",
        "//genc/proto/v0:computation_cc_proto",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)