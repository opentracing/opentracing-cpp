set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)
install(DIRECTORY include/opentracing DESTINATION include/opentracing 
            FILES_MATCHING PATTERN "*.h")

add_library(opentracing src/noop.cpp src/tracer.cpp)
add_subdirectory(test)
