TEST_NAMES = [
    "string_view_test",
    "tracer_test",
    "util_test",
    "value_test",
]

[cc_test(
    name = test_name,
    srcs = [test_name + ".cpp"],
    deps = [
        "//:opentracing",
        "//3rd_party:catch2",
    ],
) for test_name in TEST_NAMES]
