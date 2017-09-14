# - Locate the opentracing headers and libraries
# Usage:
#    find_package(OpenTracing)
#    find_package(OpenTracing 0.1.0)  # To specify a minimum version
# This will create the targets:
#    OpenTracing::opentracing # dynamic library
#    OpenTracing::opentracing-static # static library
#
# Linking to these libraries will also add the approprate interface header path to your compilation line.
# e.g. target_link_libraries(newtarget OpenTracing::opentracing)

include("${CMAKE_CURRENT_LIST_DIR}/OpenTracingTargets.cmake")
