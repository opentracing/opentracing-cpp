#include <opentracing/dynamic_load.h>
#include <opentracing/mocktracer/tracer_factory.h>
#include <cstdio>
#include <cstring>
#include <exception>

int OpenTracingMakeTracerFactory(const char* /*opentracing_version*/,
                                 const char* opentracing_abi_version,
                                 const void** error_category,
                                 void* error_message,
                                 void** tracer_factory) try {
  if (error_category == nullptr || tracer_factory == nullptr) {
    fprintf(stderr,
            "`error_category` and `tracer_factory` must be non-null.\n");
    std::terminate();
  }

  if (std::strcmp(opentracing_abi_version, OPENTRACING_ABI_VERSION) != 0) {
    *error_category =
        static_cast<const void*>(&opentracing::dynamic_load_error_category());
    auto& message = *static_cast<std::string*>(error_message);
    message =
        "incompatible OpenTracing ABI versions; "
        "expected " OPENTRACING_ABI_VERSION " but got ";
    message.append(opentracing_abi_version);
    return opentracing::incompatible_library_versions_error.value();
  }

  *tracer_factory = new opentracing::mocktracer::MockTracerFactory{};

  return 0;
} catch (const std::bad_alloc&) {
  *error_category = static_cast<const void*>(&std::generic_category());
  return static_cast<int>(std::errc::not_enough_memory);
}
