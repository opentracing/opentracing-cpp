set(CPACK_RPM_COMPONENT_INSTALL ON)
cmake_minimum_required(VERSION 3.1)

project(opentracing-cpp)

# ==============================================================================
# Version information

# Increment ABI version for any ABI-breaking change.
#
# Also, whenever the ABI is between versions and in development
# suffix the ABI version number with "_unstable".
set(OPENTRACING_ABI_VERSION "3")

# Version number follows semver
# See https://semver.org/
set(OPENTRACING_VERSION_MAJOR "1")
set(OPENTRACING_VERSION_MINOR "6")
set(OPENTRACING_VERSION_PATCH "0")
set(OPENTRACING_VERSION_STRING
  "${OPENTRACING_VERSION_MAJOR}.${OPENTRACING_VERSION_MINOR}.${OPENTRACING_VERSION_PATCH}")

# ==============================================================================
# Set up cpack

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ implementation of the OpenTracing API")
SET(CPACK_PACKAGE_VENDOR "opentracing.io")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")

SET(CPACK_PACKAGE_VERSION_MAJOR ${OPENTRACING_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${OPENTRACING_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${OPENTRACING_VERSION_PATCH})
set(CPACK_RPM_DIST_POST_INSTALL_SCRIPT_FILE ${CMAKE_SOURCE_DIR}/cmake/runldconfig)
set(CPACK_RPM_DIST_POST_UNINSTALL_SCRIPT_FILE ${CMAKE_SOURCE_DIR}/cmake/runldconfig)
set(CPACK_COMPONENTS_ALL DIST DEVEL)
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
set(CPACK_GENERATOR "RPM")
set(CPACK_COMPONENTS_IGNORE_GROUPS 1)

include(CPack)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# ==============================================================================
# Configure compilers

set(CMAKE_CXX_STANDARD 11)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything \
    -Wno-c++98-compat \
    -Wno-c++98-compat-pedantic \
    -Wno-c++98-compat-bind-to-temporary-copy \
    -Wno-weak-vtables \
    -Wno-exit-time-destructors \
    -Wno-global-constructors \
    -Wno-padded")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
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
# Check for weak symbol support

try_compile(
  SUPPORTS_WEAK_SYMBOLS
  "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/CMakeTmp"
  SOURCES ${CMAKE_CURRENT_LIST_DIR}/cmake/weak_symbol.cpp)

# ==============================================================================
# Set up options

option(BUILD_SHARED_LIBS "Build as a shared library" ON)
option(BUILD_STATIC_LIBS "Build as a static library" ON)
option(BUILD_MOCKTRACER "Build mocktracer library" ON)
option(BUILD_DYNAMIC_LOADING "Build with dynamic loading support" ON)

if (BUILD_DYNAMIC_LOADING)
  if (NOT WIN32)
    if (NOT SUPPORTS_WEAK_SYMBOLS OR NOT UNIX)
      message(WARNING "Building without dynamic loading support.")
      set(BUILD_DYNAMIC_LOADING OFF)
    endif()
  endif()
endif()

set(OPENTRACING_BUILD_DYNAMIC_LOADING ${BUILD_DYNAMIC_LOADING})

if (NOT BUILD_SHARED_LIBS AND NOT BUILD_STATIC_LIBS)
    message(FATAL_ERROR "One or both of BUILD_SHARED_LIBS or BUILD_STATIC_LIBS must be set to ON to build")
endif()

# ==============================================================================
# Set up libdir

if (NOT DEFINED LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR lib)
endif()

# ==============================================================================
# Set up generated header files config.h and version.h

configure_file(version.h.in include/opentracing/version.h)
configure_file(config.h.in include/opentracing/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/opentracing
        DESTINATION include
        COMPONENT DEVEL)

# ==============================================================================
# OpenTracing library targets

include_directories(include)
include_directories(SYSTEM 3rd_party/include)

set(SRCS src/propagation.cpp
         src/dynamic_load.cpp
         src/noop.cpp
         src/tracer.cpp
         src/tracer_factory.cpp
         src/ext/tags.cpp)

if (BUILD_DYNAMIC_LOADING)
  if (WIN32)
    list(APPEND SRCS src/dynamic_load_windows.cpp)
  else()
    list(APPEND SRCS src/dynamic_load_unix.cpp)
  endif()
else()
  list(APPEND SRCS src/dynamic_load_unsupported.cpp)
endif()

list(APPEND LIBRARIES "")
if (BUILD_DYNAMIC_LOADING)
  list(APPEND LIBRARIES ${CMAKE_DL_LIBS})
endif()


if (BUILD_SHARED_LIBS)
  add_library(opentracing SHARED ${SRCS})
  target_link_libraries(opentracing ${LIBRARIES})
  target_include_directories(opentracing INTERFACE "$<INSTALL_INTERFACE:include/>")
  set_target_properties(opentracing PROPERTIES VERSION ${OPENTRACING_VERSION_STRING}
                                             SOVERSION ${OPENTRACING_VERSION_MAJOR})
  target_compile_definitions(opentracing PRIVATE OPENTRACING_EXPORTS)
  install(TARGETS opentracing EXPORT OpenTracingTargets
          COMPONENT DIST
          RUNTIME DESTINATION ${LIB_INSTALL_DIR}
          LIBRARY DESTINATION ${LIB_INSTALL_DIR}
          ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
          )
  if (CLANG_TIDY_EXE)
    set_target_properties(opentracing PROPERTIES
                                    CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
  endif()
endif()

if (BUILD_STATIC_LIBS)
  add_library(opentracing-static STATIC ${SRCS})
  target_link_libraries(opentracing-static ${LIBRARIES})
  # Windows generates a lib and dll files for a shared library. using the same name will override the lib file generated by the shared target
  if (NOT WIN32)
    set_target_properties(opentracing-static PROPERTIES OUTPUT_NAME opentracing)
  endif()
  target_compile_definitions(opentracing-static PUBLIC OPENTRACING_STATIC)
  target_include_directories(opentracing-static INTERFACE "$<INSTALL_INTERFACE:include/>")
  install(TARGETS opentracing-static EXPORT OpenTracingTargets
      ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
endif()


install(DIRECTORY 3rd_party/include/opentracing/expected
            COMPONENT DEVEL
            DESTINATION include/opentracing
            FILES_MATCHING PATTERN "*.hpp"
                           PATTERN "*.h")
install(DIRECTORY 3rd_party/include/opentracing/variant
            COMPONENT DEVEL
            DESTINATION include/opentracing
            FILES_MATCHING PATTERN "*.hpp"
                           PATTERN "*.h")
install(DIRECTORY include/opentracing
            COMPONENT DEVEL
            DESTINATION include
            FILES_MATCHING PATTERN "*.h")


if (BUILD_MOCKTRACER)
  add_subdirectory(mocktracer)
endif()

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
set(ConfigPackageLocation ${LIB_INSTALL_DIR}/cmake/OpenTracing)
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

# ==============================================================================
# Examples

if(BUILD_TESTING)
  add_subdirectory(example)
endif()
