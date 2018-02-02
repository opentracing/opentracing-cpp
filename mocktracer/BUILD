cc_library(
    name = "mocktracer",
    srcs = glob(["src/*.cpp", "src/*.h"]),
    hdrs = glob(["include/opentracing/**/*.h"]),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        "//:opentracing",
        "//mocktracer/3rd_party/nlohmann:json",
    ],
)

cc_binary(
    name = "libmocktracer_plugin.so",
    linkshared = 1,
    visibility = ["//visibility:public"],
    deps = [
        "//mocktracer:mocktracer"
    ],
)
