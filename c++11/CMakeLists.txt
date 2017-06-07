set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything \
  -Wno-c++98-compat \
  -Wno-c++98-compat-bind-to-temporary-copy \
  -Wno-unused-parameter \
  -Wno-weak-vtables \
  -Wno-exit-time-destructors \
  -Wno-padded")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
endif()

include_directories(include)
install(DIRECTORY include/opentracing DESTINATION include/opentracing 
            FILES_MATCHING PATTERN "*.h")

add_library(opentracing src/noop.cpp src/tracer.cpp)
add_subdirectory(test)
