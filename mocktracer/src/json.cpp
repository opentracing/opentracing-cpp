#include <opentracing/mocktracer/json.h>
#include <opentracing/mocktracer/nlohmann/json.hpp>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
std::string ToJson(const std::vector<SpanData>& spans) {
  return {};
}

std::vector<SpanData> FromJson(string_view json) {
  return {};
}

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
