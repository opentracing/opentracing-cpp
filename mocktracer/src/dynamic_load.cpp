#include <opentracing/dynamic_load.h>
#include <opentracing/mocktracer/tracer_factory.h>
#include <cstdio>
#include <cstring>
#include <exception>

int OpenTracingMakeTracerFactory(const char* opentracing_version,
                                 const void** error_category,
                                 void** tracer_factory) {
  if (error_category == nullptr || tracer_factory == nullptr) {
    fprintf(stderr,
            "`error_category` and `tracer_factory` must be non-null.\n");
    std::terminate();
  }

  if (std::strcmp(opentracing_version, OPENTRACING_VERSION) != 0) {
    *error_category =
        static_cast<const void*>(&opentracing::dynamic_load_error_category());
    return opentracing::incompatible_library_versions_error.value();
  }

  *tracer_factory =
      new (std::nothrow) opentracing::mocktracer::MockTracerFactory{};
  if (*tracer_factory == nullptr) {
    *error_category = static_cast<const void*>(&std::generic_category());
    return static_cast<int>(std::errc::not_enough_memory);
  }

  return 0;
}
