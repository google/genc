package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "executor",
    srcs = ["executor.cc"],
    hdrs = ["executor.h"],
)

cc_library(
    name = "model_call_executor",
    srcs = ["model_call_executor.cc"],
    hdrs = ["model_call_executor.h"],
    deps = [":executor"],
)

cc_test(
    name = "model_call_executor_test",
    timeout = "short",
    srcs = ["model_call_executor_test.cc"],
    deps = [
        ":model_call_executor",
        "//testing/base/public:gunit_main",
    ],
)
