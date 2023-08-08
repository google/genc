package_group(
    name = "python_packages",
    packages = ["//generative_computing/python/..."],
)

py_library(
    name = "python",
    srcs = ["__init__.py"],
    visibility = [":python_packages"],
    deps = ["//generative_computing/python/interop"],
)
