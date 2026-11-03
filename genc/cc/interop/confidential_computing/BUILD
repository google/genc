load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])

cc_library(
    name = "attestation",
    srcs = ["attestation.cc"],
    hdrs = ["attestation.h"],
    deps = [
        "//genc/cc/interop/networking:curl_based_http_client",
        "//genc/cc/interop/networking:http_client_interface",
        "//genc/cc/runtime:status_macros",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@nlohmann_json//:json",
        "@tink_cc//tink:keyset_handle",
        "@tink_cc//tink/config:global_registry",
        "@tink_cc//tink/jwt:jwk_set_converter",
        "@tink_cc//tink/jwt:jwt_public_key_verify",
        "@tink_cc//tink/jwt:jwt_signature_config",
        "@tink_cc//tink/jwt:jwt_validator",
        "@tink_cc//tink/jwt:verified_jwt",
    ],
)
