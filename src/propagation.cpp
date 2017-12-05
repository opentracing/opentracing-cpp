#include <opentracing/propagation.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace {
class PropagationErrorCategory : public std::error_category {
 public:
  // Needed to fix bug in macOS build
  // (https://travis-ci.org/isaachier/hunter/jobs/281868518).
  // See https://stackoverflow.com/a/7411708/1930331 for justification.
  PropagationErrorCategory() {}

  const char* name() const noexcept override {
    return "OpenTracingPropagationError";
  }

  std::error_condition default_error_condition(int code) const
      noexcept override {
    if (code == invalid_span_context_error.value()) {
      return std::make_error_condition(std::errc::not_supported);
    }
    if (code == invalid_carrier_error.value()) {
      return std::make_error_condition(std::errc::invalid_argument);
    }
    if (code == span_context_corrupted_error.value()) {
      return std::make_error_condition(std::errc::invalid_argument);
    }
    if (code == key_not_found_error.value()) {
      return std::make_error_condition(std::errc::invalid_argument);
    }
    if (code == lookup_key_not_supported_error.value()) {
      return std::make_error_condition(std::errc::not_supported);
    }
    return std::error_condition(code, *this);
  }

  std::string message(int code) const override {
    if (code == invalid_span_context_error.value()) {
      return "opentracing: SpanContext type incompatible with tracer";
    }
    if (code == invalid_carrier_error.value()) {
      return "opentracing: Invalid Inject/Extract carrier";
    }
    if (code == span_context_corrupted_error.value()) {
      return "opentracing: SpanContext data corrupted in Extract carrier";
    }
    if (code == key_not_found_error.value()) {
      return "opentracing: SpanContext key not found";
    }
    if (code == lookup_key_not_supported_error.value()) {
      return "opentracing: Lookup for the given key is not supported";
    }
    return "opentracing: unknown propagation error";
  }
};
}  // anonymous namespace

const std::error_category& propagation_error_category() {
  static const PropagationErrorCategory error_category;
  return error_category;
}
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
