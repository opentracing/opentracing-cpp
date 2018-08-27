#ifndef OPENTRACING_MOCKTRACER_JSON_H
#define OPENTRACING_MOCKTRACER_JSON_H

#include <opentracing/mocktracer/recorder.h>
#include <opentracing/mocktracer/symbols.h>
#include <vector>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
// Serialize provided spans to JSON.
OPENTRACING_MOCK_TRACER_API void ToJson(std::ostream& writer,
                                        const std::vector<SpanData>& spans);
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_JSON_H
