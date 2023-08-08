py_library(
    name = "langchain",
    srcs = ["__init__.py"],
    visibility = ["//generative_computing/python:python_packages"],
    deps = [
        ":create_computation",
        ":custom_model",
    ],
)

py_library(
    name = "create_computation",
    srcs = ["create_computation.py"],
)

py_test(
    name = "create_computation_test",
    srcs = ["create_computation_test.py"],
    deps = [
        ":create_computation",
        ":custom_model",
    ],
)

py_library(
    name = "custom_model",
    srcs = ["custom_model.py"],
)

py_test(
    name = "custom_model_test",
    srcs = ["custom_model_test.py"],
    deps = [":custom_model"],
)
