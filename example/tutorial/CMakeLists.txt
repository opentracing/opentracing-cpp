if (BUILD_MOCKTRACER AND BUILD_SHARED_LIBS)
  add_executable(tutorial-example tutorial-example.cpp)
  target_link_libraries(tutorial-example opentracing_mocktracer)
endif()
