add_executable(tracer_test tracer_test.cpp)
target_link_libraries(tracer_test opentracing) 
add_test(tracer_test tracer_test)

add_executable(stringref_test stringref_test.cpp)
add_test(stringref_test stringref_test)

add_executable(value_test value_test.cpp)
add_test(value_test value_test)

add_executable(util_test util_test.cpp)
add_test(util_test util_test)
