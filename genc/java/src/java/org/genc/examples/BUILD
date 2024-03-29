package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

java_binary(
    name = "genc_demo",
    srcs = ["GencDemo.java"],
    main_class = "org.genc.examples.GencDemo",
    deps = [
        "//genc/java/src/java/org/genc/authoring:constructor",
        "//genc/java/src/java/org/genc/examples/executors:default_executor",
        "//genc/java/src/java/org/genc/runtime:runner",
        "//genc/proto/v0:computation_java_proto_lite",
    ],
)
