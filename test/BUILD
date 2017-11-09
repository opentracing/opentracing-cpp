TEST_NAMES = [
    "string_view_test",
    "tracer_test",
    "util_test",
    "value_test",
]

[cc_test(
    name = test_name,
    srcs = [test_name + ".cpp"],
    deps = ["//:opentracing"],
) for test_name in TEST_NAMES]
