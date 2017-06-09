add_executable(noop_test noop_test.cpp)
target_link_libraries(noop_test opentracing) 
add_test(noop_test noop_test)

add_executable(stringref_test stringref_test.cpp)
add_test(stringref_test stringref_test)
