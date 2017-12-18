#ifndef OPENTRACING_DYNAMIC_LOAD_H
#define OPENTRACING_DYNAMIC_LOAD_H

#include <opentracing/tracer.h>
#include <opentracing/tracer_factory.h>
#include <opentracing/version.h>
#include <system_error>

// opentracing_make_tracer_factory provides common hook that can be used to
// create a TracerFactory from a dynamically loaded library. Users should prefer
// to use the function dynamically_load_tracing_library over calling it
// directly.
//
// It takes the parameter `opentracing_version` representing the version of
// opentracing used by the caller. Upon success it returns the code `0` and
// sets `tracer_factory` to point to an instance of TracerFactory.
//
// On failure, it returns a non-zero error code and sets `error_category` to
// point to an std::error_category for the returned error code.
//
// Example usage,
//
//   const std::error_category* error_category = nullptr;
//   opentracing::TracerFactory* tracer_factory = nullptr;
//   int rcode = opentracing_make_factory(
//                  OPENTRACING_VERSION,
//                  &static_cast<const void*>(error_category),
//                  &static_cast<void*>(tracer_factory));
//   if (rcode == 0) {
//      // success
//      assert(tracer_factory != nullptr);
//   } else {
//      // failure
//      assert(error_category != 0);
//      std::error_code error{rcode, *error_cateogry};
//   }
extern "C" {
#ifdef _MSC_VER
__declspec(selectany) int (*opentracing_make_tracer_factory)(
    const char* opentracing_version, const void** error_category,
    void** tracer_factory);
#else
int __attribute((weak))
opentracing_make_tracer_factory(const char* opentracing_version,
                                const void** error_category,
                                void** tracer_factory);
#endif
}  // extern "C"

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
