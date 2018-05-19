#include <opentracing/dynamic_load.h>
#include <opentracing/version.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace {
class DynamicLoadErrorCategory : public std::error_category {
 public:
  DynamicLoadErrorCategory() {}

  const char* name() const noexcept override {
    return "OpenTracingDynamicLoadError";
  }

  std::error_condition default_error_condition(int code) const
      noexcept override {
    if (code == dynamic_load_failure_error.value()) {
      return std::make_error_condition(std::errc::no_such_file_or_directory);
    }
    if (code == dynamic_load_not_supported_error.value()) {
      return std::make_error_condition(std::errc::not_supported);
    }
    if (code == incompatible_library_versions_error.value()) {
      return std::make_error_condition(std::errc::invalid_argument);
    }
    return std::error_condition(code, *this);
  }

  std::string message(int code) const override {
    if (code == dynamic_load_failure_error.value()) {
      return "opentracing: failed to load dynamic library";
    }
    if (code == dynamic_load_not_supported_error.value()) {
      return "opentracing: dynamic library loading is not supported";
    }
    if (code == incompatible_library_versions_error.value()) {
      return "opentracing: versions of opentracing libraries are incompatible";
    }
    return "opentracing: unknown dynamic load error";
  }
};
}  // anonymous namespace

const std::error_category& dynamic_load_error_category() {
  static const DynamicLoadErrorCategory error_category;
  return error_category;
}

DynamicTracingLibraryHandle::DynamicTracingLibraryHandle(
    std::unique_ptr<const TracerFactory>&& tracer_factory,
    std::unique_ptr<DynamicLibraryHandle>&& dynamic_library_handle) noexcept
    : dynamic_library_handle_{std::move(dynamic_library_handle)},
      tracer_factory_{std::move(tracer_factory)} {}

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
