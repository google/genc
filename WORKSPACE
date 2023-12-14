workspace(name = "org_generative_computing")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    tag = "1.3.0",
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    tag = "v3.21.9",
)

git_repository(
    name = "pantor_inja",
    remote = "https://github.com/pantor/inja.git",
    tag = "v3.4.0",
    build_file = "//bazel:inja.BUILD",
)

git_repository(
    name = "nlohmann_json",
    remote = "https://github.com/nlohmann/json.git",
    tag = "v3.11.3",
)

git_repository(
    name = "rules_license",
    remote = "https://github.com/bazelbuild/rules_license.git",
    tag = "0.0.4",
)

git_repository(
    name = "rules_python",
    remote = "https://github.com/bazelbuild/rules_python.git",
    tag = "0.5.0",
)

http_archive(
    name = "rules_pkg",
    sha256 = "8a298e832762eda1830597d64fe7db58178aa84cd5926d76d5b744d6558941c2",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_pkg/releases/download/0.7.0/rules_pkg-0.7.0.tar.gz",
        "https://github.com/bazelbuild/rules_pkg/releases/download/0.7.0/rules_pkg-0.7.0.tar.gz",
    ],
)

git_repository(
    name = "com_github_grpc_grpc",
    remote = "https://github.com/grpc/grpc.git",
    tag = "v1.43.0",
)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    tag = "release-1.11.0",
)

http_archive(
    name = "com_google_absl",
    sha256 = "3ea49a7d97421b88a8c48a0de16c16048e17725c7ec0f1d3ea2683a2a75adc21",
    strip_prefix = "abseil-cpp-20230125.0",
    url = "https://github.com/abseil/abseil-cpp/archive/refs/tags/20230125.0.tar.gz",
)

http_archive(
    name = "pybind11_bazel",
    strip_prefix = "pybind11_bazel-203508e14aab7309892a1c5f7dd05debda22d9a5",
    urls = ["https://github.com/pybind/pybind11_bazel/archive/203508e14aab7309892a1c5f7dd05debda22d9a5.zip"],
    sha256 = "75922da3a1bdb417d820398eb03d4e9bd067c4905a4246d35a44c01d62154d91",
)

http_archive(
  name = "curl",
  urls = ["https://curl.se/download/curl-8.4.0.tar.gz"],
  sha256 = "816e41809c043ff285e8c0f06a75a1fa250211bbfb2dc0a037eeef39f1a9e427",
  strip_prefix = "curl-8.4.0",
  build_file = "//bazel:curl.BUILD",
)


new_git_repository(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11.BUILD",
    remote = "https://github.com/pybind/pybind11.git",
    tag = "v2.9.2",
)

load("@pybind11_bazel//:python_configure.bzl", "python_configure")

python_configure(name = "local_config_python")

git_repository(
    name = "pybind11_abseil",
    commit = "38111ef06d426f75bb335a3b58aa0342f6ce0ce3",
    remote = "https://github.com/pybind/pybind11_abseil.git",
)

git_repository(
    name = "pybind11_protobuf",
    commit = "80f3440cd8fee124e077e2e47a8a17b78b451363",
    remote = "https://github.com/pybind/pybind11_protobuf.git",
)

http_archive(
    name = "com_github_google_re2",
    sha256 = "8b4a8175da7205df2ad02e405a950a02eaa3e3e0840947cd598e92dca453199b",
    strip_prefix = "re2-2023-06-01",
    urls = [
        "https://github.com/google/re2/archive/refs/tags/2023-06-01.tar.gz",
        "https://storage.googleapis.com/mirror.tensorflow.org/github.com/google/re2/archive/refs/tags/2023-06-01.tar.gz",
    ],
)
