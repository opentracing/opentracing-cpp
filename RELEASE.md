# Release Process

1. Update CMakeLists.txt 
    * Set OPENTRACING_VERSION_MAJOR, OPENTRACING_VERSION_MINOR, and OPENTRACING_VERSION_PATCH to the correct numbers.
    * Remove `_unstable` from OPENTRACING_ABI_VERSION if present.
2. Create a PR "Preparing for release X.Y.Z" against master branch
3. Create a release "Release X.Y.Z" on Github
    * Create Tag `vX.Y.Z`.
