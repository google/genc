package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

package_group(
    name = "genc_demo_packages",
    packages = ["//generative_computing/java/src/java/org/generativecomputing/examples/apps/gencdemo/..."],
)

android_library(
    name = "lib",
    srcs = [
        "DefaultAndroidExecutor.java",
        "GencDemo.java",
    ],
    custom_package = "org.generativecomputing.examples.apps.gencdemo",
    manifest = "AndroidManifest.xml",
    resource_files = glob(["res/**/*"]),
    visibility = ["//visibility:public"],
    deps = [
        "//generative_computing/java/src/java/org/generativecomputing:native",
        "//generative_computing/java/src/java/org/generativecomputing:runner",
        "//generative_computing/java/src/java/org/generativecomputing/examples/apps/gencdemo/lib:jni",
        "//generative_computing/java/src/java/org/generativecomputing/interop/backends/openai:open_ai_client",
        "//generative_computing/java/src/java/org/generativecomputing/interop/network:cronet_engine_provider",
        "//generative_computing/java/src/java/org/generativecomputing/interop/network:http_client",
        "//generative_computing/proto/v0:computation_java_proto_lite",
        "//generative_computing/proto/v0:executor_java_proto_lite",
        "@com_google_protobuf//java/lite",
        "@maven//:androidx_appcompat_appcompat",
        "@maven//:androidx_legacy_legacy_support_v4",
        "@maven//:com_google_flogger_flogger",  # unuseddeps: keep
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
