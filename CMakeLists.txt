cmake_minimum_required(VERSION 3.1)

project(opentracing-cpp)

# ==============================================================================
# Version information

set(OPENTRACING_ABI_VERSION "1")
set(OPENTRACING_VERSION_MAJOR "1")
set(OPENTRACING_VERSION_MINOR "3")
set(OPENTRACING_VERSION_PATCH "0")
set(OPENTRACING_VERSION_STRING
  "${OPENTRACING_VERSION_MAJOR}.${OPENTRACING_VERSION_MINOR}.${OPENTRACING_VERSION_PATCH}")

# ==============================================================================
# Set up cpack

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ implementation of the OpenTracing API")
SET(CPACK_PACKAGE_VENDOR "opentracing.io")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

SET(CPACK_PACKAGE_VERSION_MAJOR ${OPENTRACING_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${OPENTRACING_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${OPENTRACING_VERSION_PATCH})
include(CPack)

# ==============================================================================
# Set up generated version.h

configure_file(version.h.in include/opentracing/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/opentracing
        DESTINATION include)

# ==============================================================================
# Configure compilers

set(CMAKE_CXX_STANDARD 11)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything \
    -Wno-c++98-compat \
    -Wno-c++98-compat-bind-to-temporary-copy \
    -Wno-weak-vtables \
    -Wno-exit-time-destructors \
    -Wno-global-constructors \
    -Wno-padded")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# ==============================================================================
# Set up linter

option(ENABLE_LINTING "Run clang-tidy on source files" ON)
if(ENABLE_LINTING)
  find_program(CLANG_TIDY_EXE NAMES "clang-tidy"
                              DOC "Path to clang-tidy executable")
  if(NOT CLANG_TIDY_EXE)
    message(STATUS "clang-tidy not found.")
  else()
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks=*,-clang-analyzer-alpha.*")
  endif()
endif()

# ==============================================================================
# OpenTracing library targets

include_directories(include)
include_directories(SYSTEM 3rd_party/include)

option(BUILD_SHARED_LIBS "Build as a shared library" ON)
option(BUILD_STATIC_LIBS "Build as a static library" ON)

if (NOT BUILD_SHARED_LIBS AND NOT BUILD_STATIC_LIBS)
    message(FATAL_ERROR "One or both of BUILD_SHARED_LIBS or BUILD_STATIC_LIBS must be set to ON to build")
endif()

set(SRCS src/propagation.cpp src/noop.cpp src/tracer.cpp)

if (BUILD_SHARED_LIBS)
  add_library(opentracing SHARED ${SRCS})
  target_include_directories(opentracing INTERFACE "$<INSTALL_INTERFACE:include/>")
  set_target_properties(opentracing PROPERTIES VERSION ${OPENTRACING_VERSION_STRING}
                                             SOVERSION ${OPENTRACING_VERSION_MAJOR})
  install(TARGETS opentracing EXPORT OpenTracingTargets
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)
  if (CLANG_TIDY_EXE)
    set_target_properties(opentracing PROPERTIES
                                    CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
  endif()
endif(BUILD_SHARED_LIBS)

if (BUILD_STATIC_LIBS)
  add_library(opentracing-static STATIC ${SRCS})
  set_target_properties(opentracing-static PROPERTIES OUTPUT_NAME opentracing)
  target_include_directories(opentracing-static INTERFACE "$<INSTALL_INTERFACE:include/>")
  install(TARGETS opentracing-static EXPORT OpenTracingTargets
          ARCHIVE DESTINATION lib)
endif(BUILD_STATIC_LIBS)


install(DIRECTORY 3rd_party/include/opentracing DESTINATION include
            FILES_MATCHING PATTERN "*.hpp"
                           PATTERN "*.h")
install(DIRECTORY include/opentracing DESTINATION include
            FILES_MATCHING PATTERN "*.h")


# ==============================================================================
# Package configuration setup

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/OpenTracingConfigVersion.cmake"
    VERSION ${OPENTRACING_VERSION_STRING}
    COMPATIBILITY AnyNewerVersion)
export(EXPORT OpenTracingTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/OpenTracingTargets.cmake"
    NAMESPACE OpenTracing::)
configure_file(cmake/OpenTracingConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/OpenTracingConfig.cmake"
    COPYONLY)
set(ConfigPackageLocation lib/cmake/OpenTracing)
install(EXPORT OpenTracingTargets
    FILE OpenTracingTargets.cmake
    NAMESPACE OpenTracing::
    DESTINATION ${ConfigPackageLocation})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OpenTracingConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/OpenTracingConfigVersion.cmake"
    DESTINATION ${ConfigPackageLocation}
    COMPONENT Devel)

# ==============================================================================
# Testing

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
