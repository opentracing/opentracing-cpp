cc_library(
    name = "mocktracer",
    srcs = glob(["src/*.cpp", "src/*.h"]),
    hdrs = glob(["include/opentracing/**/*.h"]),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        "//:opentracing",
        "//mocktracer/3rd_party/base64:base64",
        "//mocktracer/3rd_party/nlohmann:json",
    ],
)
