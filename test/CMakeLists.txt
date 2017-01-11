include(CheckCXXCompilerFlag)

set(unittest_sources
    baggage.t.cc
    carriers.t.cc
    span.t.cc
    spancontext.t.cc
    stringref.t.cc
    tracer.t.cc
    unittest.t.cc)

option(enable_noop "Test noop tracer." OFF)

if(enable_noop)
    set(unittest_sources ${unittest_sources} noop.t.cc)
    include_directories(${CMAKE_SOURCE_DIR})
    set(noop_lib "opentracing")
endif()

include_directories(${CMAKE_SOURCE_DIR})

add_executable(unittest ${unittest_sources})

target_link_libraries(unittest ${noop_lib} gtest gtest_main)

add_test(NAME unittest
         COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unittest
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
