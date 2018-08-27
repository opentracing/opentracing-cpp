if (BUILD_SHARED_LIBS)
  set(OPENTRACING_LIBRARY opentracing)
else()
  set(OPENTRACING_LIBRARY opentracing-static)
endif()

add_executable(tracer_test tracer_test.cpp)
target_link_libraries(tracer_test ${OPENTRACING_LIBRARY}) 
add_test(NAME tracer_test COMMAND tracer_test)

add_executable(string_view_test string_view_test.cpp)
add_test(NAME string_view_test COMMAND string_view_test)

add_executable(value_test value_test.cpp)
add_test(NAME value_test COMMAND value_test)

add_executable(util_test util_test.cpp)
add_test(NAME util_test COMMAND util_test)

if (BUILD_SHARED_LIBS AND BUILD_MOCKTRACER AND BUILD_DYNAMIC_LOADING)
  add_executable(dynamic_load_test dynamic_load_test.cpp)
  target_link_libraries(dynamic_load_test ${OPENTRACING_LIBRARY}) 
  add_dependencies(dynamic_load_test opentracing_mocktracer)
  add_test(NAME dynamic_load_test COMMAND dynamic_load_test 
    --mocktracer_library 
    $<TARGET_FILE:opentracing_mocktracer>)

  add_executable(multiple_tracer_link_test multiple_tracer_link_test.cpp
                                           tracer_a.cpp tracer_b.cpp)
  target_link_libraries(multiple_tracer_link_test ${OPENTRACING_LIBRARY}) 
  add_test(NAME multiple_tracer_link_test COMMAND multiple_tracer_link_test)
endif()
