set(CMAKE_CXX_STANDARD 11)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything \
    -Wno-c++98-compat \
    -Wno-c++98-compat-bind-to-temporary-copy \
    -Wno-unused-parameter \
    -Wno-weak-vtables \
    -Wno-exit-time-destructors \
    -Wno-global-constructors \
    -Wno-padded")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
endif()

configure_file(version.h.in include/opentracing/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/opentracing 
        DESTINATION include)

include_directories(SYSTEM 3rd_party/include)
install(DIRECTORY 3rd_party/include/opentracing DESTINATION include
            FILES_MATCHING PATTERN "*.hpp"
                           PATTERN "*.h")

include_directories(include)
install(DIRECTORY include/opentracing DESTINATION include
            FILES_MATCHING PATTERN "*.h")

set(SRCS src/propagation.cpp src/noop.cpp src/tracer.cpp)
add_library(opentracing SHARED ${SRCS})
add_library(opentracing-static STATIC ${SRCS})
set_target_properties(opentracing-static PROPERTIES OUTPUT_NAME opentracing)
install(TARGETS opentracing opentracing-static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

add_subdirectory(test)
