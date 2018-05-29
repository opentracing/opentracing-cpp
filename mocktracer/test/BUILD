TEST_NAMES = [
    "propagation_test",
    "tracer_test",
    "tracer_factory_test",
    "json_test",
]

[cc_test(
    name = test_name,
    srcs = [test_name + ".cpp"],
    deps = [
        "//mocktracer:mocktracer",
        "//3rd_party:catch2",
    ],
) for test_name in TEST_NAMES]
