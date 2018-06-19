#ifndef OPENTRACING_DYNAMIC_LOAD_H
#define OPENTRACING_DYNAMIC_LOAD_H

#include <opentracing/config.h>
#include <opentracing/symbols.h>
#include <opentracing/tracer.h>
#include <opentracing/tracer_factory.h>
#include <opentracing/version.h>
#include <system_error>

// OpenTracingMakeTracerFactory provides a common hook that can be used to
// create an TracerFactory from a dynamically loaded library. Users should
// prefer to use the function DynamicallyLoadTracingLibrary over calling it
// directly.
//
// It takes the parameter `opentracing_version` and `opentracing_abi_version`
// representing the version of opentracing used by the caller. Upon success it
// returns the code `0` and sets `tracer_factory` to point to an instance of
// TracerFactory.
//
// On failure, it returns a non-zero error code and sets `error_category` to
// point to an std::error_category for the returned error code.
//
// Example usage,
//
//   const std::error_category* error_category = nullptr;
//   std::string error_message;
//   opentracing::TracerFactory* tracer_factory = nullptr;
//   int rcode = (*OpenTracingMakeTracerFactory)(
//                  OPENTRACING_VERSION,
//                  OPENTRACING_ABI_VERSION,
//                  &static_cast<const void*>(error_category),
//                  static_cast<void*>(&error_message),
//                  &static_cast<void*>(tracer_factory));
//   if (rcode == 0) {
//      // success
//      assert(tracer_factory != nullptr);
//   } else {
//      // failure
//      assert(error_category != nullptr);
//      std::error_code error{rcode, *error_category};
//   }
using OpenTracingMakeTracerFactoryType = int(
    const char* opentracing_version, const char* opentracing_abi_version,
    const void** error_category, void* error_message, void** tracer_factory);

#ifdef WIN32

#define OPENTRACING_DECLARE_IMPL_FACTORY(X)                                 \
  extern "C" {                                                              \
                                                                            \
  extern __declspec(dllexport)                                              \
      OpenTracingMakeTracerFactoryType* const OpenTracingMakeTracerFactory; \
                                                                            \
  __declspec(selectany) OpenTracingMakeTracerFactoryType* const             \
      OpenTracingMakeTracerFactory = X;                                     \
  }  // extern "C"

#else

#define OPENTRACING_DECLARE_IMPL_FACTORY(X)                                 \
  extern "C" {                                                              \
                                                                            \
  __attribute((weak)) extern OpenTracingMakeTracerFactoryType* const        \
      OpenTracingMakeTracerFactory;                                         \
                                                                            \
  OpenTracingMakeTracerFactoryType* const OpenTracingMakeTracerFactory = X; \
  }  // extern "C"

#endif

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
// Returns the std::error_category class used for opentracing dynamic loading
// errors.
//
// See
//   http://blog.think-async.com/2010/04/system-error-support-in-c0x-part-1.html
//   https://ned14.github.io/boost.outcome/md_doc_md_03-tutorial_b.html
OPENTRACING_API const std::error_category& dynamic_load_error_category();

// `dynamic_load_failure_error` occurs when dynamically loading a tracer library
// fails. Possible reasons could be the library doesn't exist or it is missing
// the required symbols.
const std::error_code dynamic_load_failure_error(1,
                                                 dynamic_load_error_category());

// `dynamic_load_not_supported_error` means dynamic loading of tracing libraries
// is not supported for the platform used.
const std::error_code dynamic_load_not_supported_error(
    2, dynamic_load_error_category());

// `incompatible_library_versions_error` occurs if the tracing library
// dynamically loaded uses an incompatible version of opentracing.
const std::error_code incompatible_library_versions_error(
    3, dynamic_load_error_category());

class DynamicLibraryHandle {
 public:
  virtual ~DynamicLibraryHandle() = default;
};

// Provides a handle to a dynamically loaded tracing library that can be used
// to create tracers.
//
// Note: The handle must not be destructed while any associated tracers are
// still in use.
//
// See TracerFactory
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
  std::unique_ptr<DynamicLibraryHandle> dynamic_library_handle_;
  std::unique_ptr<const TracerFactory> tracer_factory_;
};

// Dynamically loads a tracing library and returns a handle that can be used
// to create tracers.
//
// Example:
//   std::string error_message;
//   auto handle_maybe = DynamicallyLoadTracingLibrary(
//                                 "libtracing_vendor.so",
//                                 error_message);
//   if (handle_maybe) {
//      // success
//      auto& tracer_factory = handle_maybe->tracer_factory();
//   } else {
//      // failure
//      std::error_code error = handle_maybe.error();
//      // `error_message` may also contain a more descriptive message
//   }
//
// See DynamicTracingLibraryHandle, TracerFactory
OPENTRACING_API expected<DynamicTracingLibraryHandle>
DynamicallyLoadTracingLibrary(const char* shared_library,
                              std::string& error_message) noexcept;
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_DYNAMIC_LOAD_H
