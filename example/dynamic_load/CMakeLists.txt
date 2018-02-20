if (BUILD_DYNAMIC_LOADING AND BUILD_SHARED_LIBS)
  add_executable(dynamic_load-example dynamic_load-example.cpp)
  target_link_libraries(dynamic_load-example opentracing)
endif()
