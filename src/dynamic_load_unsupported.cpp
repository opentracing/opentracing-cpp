#include <opentracing/dynamic_load.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
expected<DynamicTracingLibraryHandle> DynamicallyLoadTracingLibrary(
    const char* /*shared_library*/, std::string& /*error_message*/) noexcept {
  return make_unexpected(dynamic_load_not_supported_error);
}
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
