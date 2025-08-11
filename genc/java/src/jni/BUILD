# Java native interfaces for GenC components.

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

exports_files(
    srcs = [
        "executor_jni.cc",
        "executor_jni.h",
        "owned_value_id_jni.cc",
        "owned_value_id_jni.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "jni_utils",
    srcs = ["jni_utils.cc"],
    hdrs = ["jni_utils.h"],
    copts = [
        "-Wno-write-strings",  # The generated JNI header fails this.
        "-Wno-writable-strings",  # The generated JNI header fails this.
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//genc/java/src/jni:copy_jni_hdr_lib",
        "@bazel_tools//tools/jdk:jni",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
    alwayslink = 1,
)

genrule(
    name = "copy_link_jni_md_header",
    srcs = ["@bazel_tools//tools/jdk:jni_md_header-linux"],
    outs = ["jni_md.h"],
    cmd = "cp -f $< $@",
)

genrule(
    name = "copy_link_jni_header",
    srcs = ["@bazel_tools//tools/jdk:jni_header"],
    outs = ["jni.h"],
    cmd = "cp -f $< $@",
)

cc_library(
    name = "copy_jni_hdr_lib",
    hdrs = [
        ":copy_link_jni_header",
        ":copy_link_jni_md_header",
    ],
    includes = ["."],
)
