def cc_posix_binary(name, srcs = [], **kargs):
    native.cc_binary(
        name = name,
        srcs = select({
            "@bazel_tools//src/conditions:windows": [],
            "//conditions:default": srcs,
        }),
        **kargs
    )

# Used to specify a library that only builds on Windows
def cc_windows_binary(name, srcs = [], **kargs):
    native.cc_binary(
        name = name,
        srcs = select({
            "@bazel_tools//src/conditions:windows": srcs,
            "//conditions:default": [],
        }),
        **kargs
    )
