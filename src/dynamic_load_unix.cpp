#include <dlfcn.h>
#include <opentracing/dynamic_load.h>
#include <opentracing/version.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace {
class DynamicLibraryHandleUnix : public DynamicLibraryHandle {
 public:
  explicit DynamicLibraryHandleUnix(void* handle) : handle_{handle} {}

  ~DynamicLibraryHandleUnix() override { dlclose(handle_); }

 private:
  void* handle_;
};
}  // namespace

// Undefined behavior sanitizer has a bug where it will produce a false positive
// when casting the result of dlsym to a function pointer.
//
// See https://github.com/envoyproxy/envoy/pull/2252#issuecomment-362668221
//     https://github.com/google/sanitizers/issues/911
//
// Note: undefined behavior sanitizer is supported in clang and gcc > 4.9
#if defined(__clang__)
__attribute__((no_sanitize("function")))
// Copied from https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
#elif defined(__GNUC__) && \
    ((__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= 40900)
__attribute__((no_sanitize_undefined))
#endif
expected<DynamicTracingLibraryHandle>
DynamicallyLoadTracingLibrary(const char* shared_library,
                              std::string& error_message) noexcept try {
  dlerror();  // Clear any existing error.

  const auto handle = dlopen(shared_library, RTLD_NOW | RTLD_LOCAL);
  if (handle == nullptr) {
    error_message = dlerror();
    return make_unexpected(dynamic_load_failure_error);
  }

  std::unique_ptr<DynamicLibraryHandle> dynamic_library_handle{
      new DynamicLibraryHandleUnix{handle}};

  const auto make_tracer_factory =
      reinterpret_cast<OpenTracingMakeTracerFactoryType**>(
          dlsym(handle, "OpenTracingMakeTracerFactory"));
  if (make_tracer_factory == nullptr) {
    error_message = dlerror();
    return make_unexpected(dynamic_load_failure_error);
  }

  if (*make_tracer_factory == nullptr) {
    error_message =
        "An error occurred while looking up for OpenTracingMakeTracerFactory. "
        "It seems that it was set to nullptr.";
    return make_unexpected(dynamic_load_failure_error);
  }

  const void* error_category = nullptr;
  void* tracer_factory = nullptr;
  const auto rcode = (*make_tracer_factory)(
      OPENTRACING_VERSION, OPENTRACING_ABI_VERSION, &error_category,
      static_cast<void*>(&error_message), &tracer_factory);
  if (rcode != 0) {
    if (error_category == nullptr) {
      error_message = "failed to construct a TracerFactory: unknown error code";
      return make_unexpected(dynamic_load_failure_error);
    }
    const auto error_code = std::error_code{
        rcode, *static_cast<const std::error_category*>(error_category)};
    if (error_message.empty()) {
      error_message = error_code.message();
    }
    return make_unexpected(dynamic_load_failure_error);
  }

  if (tracer_factory == nullptr) {
    error_message =
        "failed to construct a TracerFactory: `tracer_factory` is null";
    return make_unexpected(dynamic_load_failure_error);
  }

  return DynamicTracingLibraryHandle{
      std::unique_ptr<const TracerFactory>{
          static_cast<TracerFactory*>(tracer_factory)},
      std::move(dynamic_library_handle)};
} catch (const std::bad_alloc&) {
  return make_unexpected(std::make_error_code(std::errc::not_enough_memory));
}
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
