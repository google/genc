load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_binary")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_binary(
    name = "genc_demo",
    srcs = ["genc_demo.cc"],
    linkopts = ["--bind"],
    tags = [
        "manual",
    ],
)

wasm_cc_binary(
    name = "genc_demo_wasm",
    cc_target = ":genc_demo",
    outputs = ["genc_demo.wasm"],
)
