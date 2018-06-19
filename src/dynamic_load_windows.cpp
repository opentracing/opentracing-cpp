#include <opentracing/dynamic_load.h>
#include <opentracing/version.h>

#include <windows.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
// Returns the last Win32 error, in string format. Returns an empty string if
// there is no error.
//
// Taken from https://stackoverflow.com/a/17387176/4447365
static std::string GetLastErrorAsString() {
  // Get the error message, if any.
  DWORD errorMessageID = ::GetLastError();
  if (errorMessageID == 0)
    return std::string();  // No error message has been recorded

  LPSTR messageBuffer = nullptr;
  size_t size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR)&messageBuffer, 0, NULL);

  std::string message(messageBuffer, size);

  // Free the buffer.
  LocalFree(messageBuffer);

  return message;
}

namespace {
class DynamicLibraryHandleWindows : public DynamicLibraryHandle {
 public:
  explicit DynamicLibraryHandleWindows(HINSTANCE handle) : handle_{handle} {}

  ~DynamicLibraryHandleWindows() override { FreeLibrary(handle_); }

 private:
  HINSTANCE handle_;
};
}  // namespace

expected<DynamicTracingLibraryHandle> DynamicallyLoadTracingLibrary(
    const char* shared_library, std::string& error_message) noexcept try {
  const auto handle = LoadLibrary(shared_library);
  if (handle == nullptr) {
    error_message = "An error occurred: " + GetLastErrorAsString();
    return make_unexpected(dynamic_load_failure_error);
  }

  std::unique_ptr<DynamicLibraryHandle> dynamic_library_handle{
      new DynamicLibraryHandleWindows{handle}};

  const auto make_tracer_factory =
      reinterpret_cast<OpenTracingMakeTracerFactoryType**>(
          GetProcAddress(handle, "OpenTracingMakeTracerFactory"));

  if (make_tracer_factory == nullptr) {
    error_message =
        "An error occurred while looking up for OpenTracingMakeTracerFactory "
        ": " +
        GetLastErrorAsString();
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
    return make_unexpected(error_code);
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
