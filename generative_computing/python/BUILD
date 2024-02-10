load("@rules_python//python:defs.bzl", "py_library")

package_group(
    name = "python_packages",
    packages = [
        "//generative_computing/...",
    ],
)

py_library(
    name = "python",
    srcs = ["__init__.py"],
    visibility = [
        "//visibility:private",  # Only private by automation, not intent. Owner may accept CLs adding visibility. See go/scheuklappen#explicit-private.
    ],
)
