include_directories(include)
include_directories(3rd_party/base64/include)

set(SRCS 3rd_party/base64/src/base64.cpp
         src/mock_span_context.cpp
         src/mock_span.cpp
         src/in_memory_recorder.cpp
         src/propagation.cpp
         src/tracer.cpp)

add_library(opentracing_mocktracer SHARED ${SRCS})
target_include_directories(opentracing_mocktracer INTERFACE "$<INSTALL_INTERFACE:include/>")
set_target_properties(opentracing_mocktracer PROPERTIES VERSION ${OPENTRACING_VERSION_STRING}
                                             SOVERSION ${OPENTRACING_VERSION_MAJOR})
target_link_libraries(opentracing_mocktracer opentracing)

add_library(opentracing_mocktracer-static STATIC ${SRCS})
set_target_properties(opentracing_mocktracer-static PROPERTIES OUTPUT_NAME opentracing_mocktracer)
target_include_directories(opentracing_mocktracer-static INTERFACE "$<INSTALL_INTERFACE:include/>")
target_link_libraries(opentracing_mocktracer-static opentracing-static)

# ==============================================================================
# Testing

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
