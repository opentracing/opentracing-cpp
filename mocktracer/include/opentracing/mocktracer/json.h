#ifndef OPENTRACING_MOCKTRACER_JSON_H
#define OPENTRACING_MOCKTRACER_JSON_H

#include <opentracing/mocktracer/recorder.h>
#include <vector>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
// Serialize provided spans to JSON.
void ToJson(std::ostream& writer, const std::vector<SpanData>& spans);
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_JSON_H
