include_directories(include)

set(SRCS src/mock_span_context.cpp
         src/mock_span.cpp
         src/in_memory_recorder.cpp
         src/json_recorder.cpp
         src/base64.cpp
         src/propagation.cpp
         src/json.cpp
         src/tracer.cpp
         src/tracer_factory.cpp)

if (BUILD_SHARED_LIBS)
  add_library(opentracing_mocktracer SHARED ${SRCS} src/dynamic_load.cpp)
  target_include_directories(opentracing_mocktracer INTERFACE "$<INSTALL_INTERFACE:include/>")
  set_target_properties(opentracing_mocktracer PROPERTIES VERSION ${OPENTRACING_VERSION_STRING}
                                               SOVERSION ${OPENTRACING_VERSION_MAJOR})
  target_link_libraries(opentracing_mocktracer opentracing)
  install(TARGETS opentracing_mocktracer EXPORT OpenTracingTargets
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)
endif()

if (BUILD_STATIC_LIBS)
  add_library(opentracing_mocktracer-static STATIC ${SRCS})
  set_target_properties(opentracing_mocktracer-static PROPERTIES OUTPUT_NAME opentracing_mocktracer)
  target_include_directories(opentracing_mocktracer-static INTERFACE "$<INSTALL_INTERFACE:include/>")
  target_link_libraries(opentracing_mocktracer-static opentracing-static)
  install(TARGETS opentracing_mocktracer-static EXPORT OpenTracingTargets
          ARCHIVE DESTINATION lib)
endif()

install(DIRECTORY include/opentracing DESTINATION include
            FILES_MATCHING PATTERN "*.h")

# ==============================================================================
# Testing

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
