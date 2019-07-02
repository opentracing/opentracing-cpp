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

cc_test(
    name = "mutiple_tracer_link_test",
    srcs = [
        "multiple_tracer_link_test.cpp",
        "tracer_a.cpp",
        "tracer_b.cpp",
    ],
    deps = [
        "//:opentracing",
    ],
    # when on Windows, we need to pass /DWIN32 to the compiler so that
    # we use the Windows-compatible version of dynamic_load.h
    copts = select({
      "@bazel_tools//src/conditions:windows": [ "/DWIN32" ],
      "//conditions:default": [ ]
    })
)


cc_test(
  name = "dynamic_load_test",
  srcs = ["dynamic_load_test.cpp"],

  deps = [
    # depend on opentracing shared library
    "//:opentracing",
    "//mocktracer:mocktracer",
    # depend on mocktracer shared library
    "//mocktracer:libmocktracer_plugin.so",
    "//3rd_party:catch2", # for catch.hhp
  ],
  args = ["--mocktracer_library /Users/isaaczinda/opentracing-cpp-personal/bazel-bin/mocktracer/libmocktracer_plugin.so"]
)
