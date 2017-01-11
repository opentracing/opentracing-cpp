include_directories(${CMAKE_SOURCE_DIR})

if(enable_noop)
    set(opentracing_libs "-L${libdir} -lopentracing-cpp")
endif()

set(headers
    baggage.h
    carriers.h
    constants.h
    spancontext.h
    span.h
    spanoptions.h
    stringref.h
    tracer.h)

if(enable_noop)
    set(headers ${headers} noop.h)

    add_library(opentracing-cpp STATIC noop.cc)

    install (TARGETS opentracing-cpp
             DESTINATION ${LIB_INSTALL_DIR}
             COMPONENT dev)
endif()

install (FILES ${headers}
         DESTINATION ${INCLUDE_INSTALL_DIR}/opentracing
         COMPONENT dev)
