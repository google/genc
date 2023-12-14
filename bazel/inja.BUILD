# BUILD for inja, a C++ template engine.

cc_library(
    name = "inja",
    hdrs = ["include/inja/inja.hpp"],
    copts = ["-Wno-implicit-fallthrough"],
    deps = [
        ":internal",
        "@nlohmann_json//:json",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "internal",
    includes = ["include"],
    textual_hdrs = glob(
        ["include/inja/*.hpp"],
        exclude = ["include/inja/inja.hpp"],
    ),
    visibility = ["//visibility:private"],
)

