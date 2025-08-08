# Libraries for template engines & common templates.

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(
    default_visibility = [
        #"//genc/google:__subpackages__",
        "//visibility:public",
    ],
)

licenses(["notice"])

cc_library(
    name = "inja_status_or",
    srcs = ["inja_status_or.cc"],
    hdrs = ["inja_status_or.h"],
    copts = [
        "-fexceptions",
    ],
    features = ["-use_header_modules"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@nlohmann_json//:json",
        "@pantor_inja//:inja",
    ],
)
