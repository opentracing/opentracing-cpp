#include <opentracing/dynamic_load.h>

static int OpenTracingMakeTracerFactoryFct(
    const char* /*opentracing_version*/,
    const char* /*opentracing_abi_version*/, const void** /*error_category*/,
    void* /*error_message*/, void** /*tracer_factory*/) {
  return -1;
}

OPENTRACING_DECLARE_IMPL_FACTORY(OpenTracingMakeTracerFactoryFct);
