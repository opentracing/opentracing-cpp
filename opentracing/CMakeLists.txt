include_directories(${CMAKE_SOURCE_DIR})

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
    # Used by pc file
    set(opentracing_libs "-L${libdir} -lopentracing")

    # Updated set of included files
    set(headers ${headers} noop.h)

    # Build our opentracing library
    add_library(opentracing STATIC noop.cc)

    # Add the opentracing install target
    install (TARGETS opentracing
             DESTINATION ${LIB_INSTALL_DIR}
             COMPONENT dev)
endif()

# Install the opentracing headers into /path/to/include/opentracing/
install (FILES ${headers}
         DESTINATION ${INCLUDE_INSTALL_DIR}/opentracing
         COMPONENT dev)

# pkg-config
if("${UNIX}" OR "${CYGWIN}")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
                   @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc.in
            DESTINATION ${LIB_INSTALL_DIR}/pkgconfig
            COMPONENT pkgconfig)
endif()
