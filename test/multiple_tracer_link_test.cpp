// Links in tracer_a.o and tracer_b.o to verify that there's no multiple
// definition error from OpenTracingMakeTracerFactory.
#include <opentracing/dynamic_load.h>

extern "C" {
extern OpenTracingMakeTracerFactoryType* const OpenTracingMakeTracerFactory;
}  // extern "C"

int main() {
  // Call OpenTracingMakeTracerFactory to make sure it's not elided.
  if ((*OpenTracingMakeTracerFactory)(nullptr, nullptr, nullptr, nullptr,
                                      nullptr) != -1) {
    return -1;
  }
  return 0;
}
