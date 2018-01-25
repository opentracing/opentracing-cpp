if (BUILD_SHARED_LIBS)
  set(OPENTRACING_LIBRARY opentracing)
else()
  set(OPENTRACING_LIBRARY opentracing-static)
endif()

add_executable(tracer_test tracer_test.cpp)
target_link_libraries(tracer_test ${OPENTRACING_LIBRARY}) 
add_test(tracer_test tracer_test)

add_executable(string_view_test string_view_test.cpp)
add_test(string_view_test string_view_test)

add_executable(value_test value_test.cpp)
add_test(value_test value_test)

add_executable(util_test util_test.cpp)
add_test(util_test util_test)

if (BUILD_SHARED_LIBS AND BUILD_MOCKTRACER AND BUILD_DYNAMIC_LOADING)
  add_executable(dynamic_load_test dynamic_load_test.cpp)
  target_link_libraries(dynamic_load_test ${OPENTRACING_LIBRARY}) 
  add_dependencies(dynamic_load_test opentracing_mocktracer)
  add_test(dynamic_load_test dynamic_load_test 
    --mocktracer_library 
    ${CMAKE_BINARY_DIR}/mocktracer/${CMAKE_SHARED_LIBRARY_PREFIX}opentracing_mocktracer${CMAKE_SHARED_LIBRARY_SUFFIX})
endif()
