package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

package_group(
    name = "genc_demo_packages",
    packages = ["//genc/java/src/java/org/genc/examples/apps/gencdemo/..."],
)

android_library(
    name = "lib",
    srcs = [
        "Computations.java",
        "DefaultAndroidExecutor.java",
        "GencDemo.java",
    ],
    custom_package = "org.genc.examples.apps.gencdemo",
    manifest = "AndroidManifest.xml",
    resource_files = glob(["res/**/*"]),
    visibility = ["//visibility:public"],
    deps = [
        "//genc/java/src/java/org/genc/authoring:constructor",
        "//genc/java/src/java/org/genc/examples/apps/gencdemo/lib:jni",
        "//genc/java/src/java/org/genc/interop/backends/googleai:google_ai_client",
        "//genc/java/src/java/org/genc/interop/backends/mediapipe:llm_inference_client",
        "//genc/java/src/java/org/genc/interop/backends/openai:open_ai_client",
        "//genc/java/src/java/org/genc/interop/backends/wolframalpha:wolfram_alpha_client",
        "//genc/java/src/java/org/genc/interop/network:cronet_engine_provider",
        "//genc/java/src/java/org/genc/interop/network:http_client",
        "//genc/java/src/java/org/genc/runtime:runner",
        "//genc/proto/v0:computation_java_proto_lite",
        "//genc/proto/v0:executor_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:androidx_annotation_annotation",
        "@maven//:androidx_appcompat_appcompat",
        "@maven//:androidx_legacy_legacy_support_v4",
        "@maven//:com_google_flogger_flogger",
        "@maven//:com_google_guava_guava",
        "@maven//:org_chromium_net_cronet_api",
        "@maven//:org_chromium_net_cronet_embedded",
    ],
)

android_binary(
    name = "app",
    manifest = "AndroidManifest.xml",
    multidex = "native",
    deps = [
        ":lib",
    ],
)
