if (BUILD_MOCKTRACER AND BUILD_SHARED_LIBS)
  add_executable(tutorial tutorial.cpp)
  target_link_libraries(tutorial opentracing_mocktracer)
endif()
