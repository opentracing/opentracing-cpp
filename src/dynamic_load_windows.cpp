#include <opentracing/dynamic_load.h>
#include <opentracing/version.h>

#include <windows.h> 

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace {

 class DynamicLibraryHandleWindows : public DynamicLibraryHandle {
 public:
   explicit DynamicLibraryHandleWindows(HINSTANCE handle) : handle_{handle} {}

  ~DynamicLibraryHandleWindows() override { FreeLibrary(handle_); }

 private:
  HINSTANCE handle_;
};

}  // namespace

expected<DynamicTracingLibraryHandle>
DynamicallyLoadTracingLibrary(const char* shared_library,
                              std::string& error_message) noexcept try {
  const auto handle = LoadLibrary(shared_library);
  if (handle == nullptr) {
    error_message = "An error occurred " + GetLastError();
    return make_unexpected(dynamic_load_failure_error);
  }

  std::unique_ptr<DynamicLibraryHandle> dynamic_library_handle{
      new DynamicLibraryHandleWindows{handle}};

  const auto make_tracer_factory =
      reinterpret_cast<decltype(OpenTracingMakeTracerFactory)*>(
          GetProcAddress(handle, "OpenTracingMakeTracerFactory"));
  if (make_tracer_factory == nullptr) {
    error_message = "An error occurred whike looking up for OpenTracingMakeTracerFactory : " + GetLastError();
    return make_unexpected(dynamic_load_failure_error);
  }

  const void* error_category = nullptr;
  void* tracer_factory = nullptr;
  const auto rcode = make_tracer_factory(OPENTRACING_VERSION, &error_category,
                                         &tracer_factory);
  if (rcode != 0) {
    if (error_category != nullptr) {
      return make_unexpected(std::error_code{
          rcode, *static_cast<const std::error_category*>(error_category)});
    } else {
      error_message = "failed to construct a TracerFactory: unknown error code";
      return make_unexpected(dynamic_load_failure_error);
    }
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
