#include <opentracing/tracer_factory.h>
#include <opentracing/version.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace {
class TracerFactoryErrorCategory : public std::error_category {
 public:
  TracerFactoryErrorCategory() {}

  const char* name() const noexcept override {
    return "OpenTracingTracerFactoryError";
  }

  std::error_condition default_error_condition(int code) const
      noexcept override {
    if (code == configuration_parse_error.value()) {
      return std::make_error_condition(std::errc::invalid_argument);
    }
    if (code == invalid_configuration_error.value()) {
      return std::make_error_condition(std::errc::invalid_argument);
    }
    return std::error_condition(code, *this);
  }

  std::string message(int code) const override {
    if (code == configuration_parse_error.value()) {
      return "opentracing: failed to parse configuration";
    }
    if (code == invalid_configuration_error.value()) {
      return "opentracing: invalid configuration";
    }
    return "opentracing: unknown tracer factory error";
  }
};
}  // anonymous namespace

const std::error_category& tracer_factory_error_category() {
  static const TracerFactoryErrorCategory error_category;
  return error_category;
}

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
