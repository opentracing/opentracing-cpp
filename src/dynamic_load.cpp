#include <dlfcn.h>
#include <opentracing/dynamic_load.h>
#include <opentracing/version.h>

namespace opentracing {
namespace {
class DynamicLoadErrorCategory : public std::error_category {
 public:
   DynamicLoadErrorCategory() {}

  const char* name() const noexcept override {
    return "OpenTracingDynamicLoadError";
  }

  std::error_condition default_error_condition(int code) const
      noexcept override {
    if (code == library_not_found_error.value()) {
      return std::make_error_condition(std::errc::no_such_file_or_directory);
    }
    if (code == dynamic_load_not_supported_error.value()) {
      return std::make_error_condition(std::errc::not_supported);
    }
    if (code == configuration_parse_error.value()) {
      return std::make_error_condition(std::errc::invalid_argument);
    }
    if (code == invalid_configuration_error.value()) {
      return std::make_error_condition(std::errc::invalid_argument);
    }
    return std::error_condition(code, *this);
  }

  std::string message(int code) const override {
    // TODO
    return {};
  }
};
} // anonymous namespace
const std::error_category& dynamic_load_error_category() {
  static const DynamicLoadErrorCategory error_category;
  return error_category;
}

namespace {
struct DLHandle {
  void* handle = nullptr;

  ~DLHandle() {
    if (handle != nullptr) {
      dlclose(handle);
    }
  }
};
}  // anonymous namespace

expected<std::shared_ptr<Tracer>> dynamically_load_tracer(
    const char* shared_library, const char* json_configuration,
    std::string& error_message) noexcept {
  std::shared_ptr<Tracer> result{};
  DLHandle handle;
  handle.handle = dlopen(shared_library, RTLD_NOW);
  if (handle.handle == nullptr) {
    // TODO(rnburn): return error_code
    return result;
  }
  dlerror();  // Clear any existing error.
  auto make_tracer =
      reinterpret_cast<decltype(opentracing_make_vendor_tracer)*>(
          dlsym(handle.handle, "opentracing_make_vendor_tracer"));
  if (make_tracer == nullptr) {
    // TODO(rnburn): return error_code
    return result;
  }
  const char* error_message_ptr = nullptr;
  auto rcode = (*make_tracer)(OPENTRACING_VERSION, json_configuration,
                              &error_message_ptr, static_cast<void*>(&result));
  if (error_message_ptr != nullptr) {
    error_message.assign(error_message_ptr);
    delete error_message_ptr;
  }
  if (rcode != 0) {
    // TODO(rnburn): return error_code
  }
  return result;
}
}  // namespace opentracing
