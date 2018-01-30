cc_library(
    name = "base64",
    srcs = glob(["src/*.cpp"]),
    hdrs = glob(["include/opentracing/**/*.h"]),
    strip_include_prefix = "include",
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [
        "//:opentracing",
    ]
)
