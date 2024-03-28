package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

android_local_test(
    name = "WolframAlphaClientTest",
    srcs = ["WolframAlphaClientTest.java"],
    manifest = "AndroidManifest.xml",
    deps = [
        "//genc/java/src/java/org/genc/interop/backends/wolframalpha:wolfram_alpha_client",
        "//genc/java/src/java/org/genc/interop/network:http_client",
        "//genc/java/src/java/org/genc/interop/network/api/proto:http_request_response_java_proto_lite",
        "//genc/proto/v0:computation_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:androidx_test_ext_junit",
        "@maven//:com_google_guava_guava",
        "@maven//:com_google_truth_truth",
        "@maven//:org_mockito_mockito_core",
        "@maven//:org_robolectric_annotations",
        "@maven//:org_robolectric_robolectric",
        "@maven//:org_robolectric_shadows_framework",
        "@robolectric//bazel:android-all",
    ],
)
