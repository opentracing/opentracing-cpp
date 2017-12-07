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

expected<DynamicTracingLibraryHandle> dynamically_load_tracing_library(
    const char* shared_library, std::string& error_message) noexcept try {
  dlerror();  // Clear any existing error.

  const auto handle = dlopen(shared_library, RTLD_NOW | RTLD_LOCAL);
  if (handle == nullptr) {
    error_message = dlerror();
    return make_unexpected(dynamic_load_failure_error);
  }

  std::unique_ptr<DynamicLibraryHandle> dynamic_library_handle{
      new DynamicLibraryHandleUnix{handle}};

  const auto make_tracer_factory =
      reinterpret_cast<decltype(opentracing_make_tracer_factory)*>(
          dlsym(handle, "opentracing_make_tracer_factory"));
  if (make_tracer_factory == nullptr) {
    error_message = dlerror();
    return make_unexpected(dynamic_load_failure_error);
  }

  void* tracer_factory = nullptr;
  const auto rcode = make_tracer_factory(OPENTRACING_VERSION, &tracer_factory);
  if (rcode == incompatible_library_versions_error.value()) {
    return make_unexpected(incompatible_library_versions_error);
  }
  if (rcode != 0 || tracer_factory == nullptr) {
    return make_unexpected(internal_tracer_error);
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
