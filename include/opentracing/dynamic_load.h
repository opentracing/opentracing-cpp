#ifndef OPENTRACING_DYNAMIC_LOAD_H
#define OPENTRACING_DYNAMIC_LOAD_H

#include <opentracing/tracer.h>
#include <opentracing/tracer_factory.h>
#include <opentracing/version.h>
#include <system_error>

extern "C" int __attribute((weak))
opentracing_make_tracer_factory(const char* opentracing_version,
                                const void** error_category,
                                void** tracer_factory);

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
const std::error_category& dynamic_load_error_category();

const std::error_code dynamic_load_failure_error(1,
                                                 dynamic_load_error_category());

const std::error_code dynamic_load_not_supported_error(
    2, dynamic_load_error_category());

const std::error_code incompatible_library_versions_error(
    3, dynamic_load_error_category());

class DynamicLibraryHandle {
 public:
  virtual ~DynamicLibraryHandle() = default;
};

// Provides a handle to a dynamically loaded tracing library.
//
// Note: The handle must not be destructed while any associated tracers are
// still in use.
class DynamicTracingLibraryHandle {
 public:
  DynamicTracingLibraryHandle() = default;

  DynamicTracingLibraryHandle(
      std::unique_ptr<const TracerFactory>&& tracer_factory,
      std::unique_ptr<DynamicLibraryHandle>&& dynamic_library_handle) noexcept;

  const TracerFactory& tracer_factory() const noexcept {
    return *tracer_factory_;
  }

 private:
  std::unique_ptr<const TracerFactory> tracer_factory_;
  std::unique_ptr<DynamicLibraryHandle> dynamic_library_handle_;
};

expected<DynamicTracingLibraryHandle> dynamically_load_tracing_library(
    const char* shared_library, std::string& error_message) noexcept;
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_DYNAMIC_LOAD_H
