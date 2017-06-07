add_executable(noop_test noop_test.cpp)
target_link_libraries(noop_test opentracing) 
add_test(noop_test noop_test)
