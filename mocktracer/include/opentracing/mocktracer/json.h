#ifndef OPENTRACING_MOCKTRACER_JSON_H
#define OPENTRACING_MOCKTRACER_JSON_H

#include <opentracing/mocktracer/recorder.h>
#include <vector>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
std::string ToJson(const std::vector<SpanData>& spans);

std::vector<SpanData> FromJson(string_view s);
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_JSON_H
