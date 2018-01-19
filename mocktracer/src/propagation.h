#ifndef OPENTRACING_MOCKTRACER_PROPAGATION_H
#define OPENTRACING_MOCKTRACER_PROPAGATION_H

#include <opentracing/mocktracer/recorder.h>
#include <opentracing/propagation.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
expected<void> InjectSpanContext(std::ostream& carrier,
                                 const SpanContextData& span_context_data);

expected<bool> ExtractSpanContext(std::istream& carrier,
                                  SpanContextData& span_context_data);

expected<void> InjectSpanContext(const TextMapWriter& carrier,
                                 const SpanContextData& span_context_data);

expected<bool> ExtractSpanContext(const TextMapReader& carrier,
                                  SpanContextData& span_context_data);

expected<void> InjectSpanContext(const HTTPHeadersWriter& carrier,
                                 const SpanContextData& span_context_data);

expected<bool> ExtractSpanContext(const HTTPHeadersReader& carrier,
                                  SpanContextData& span_context_data);
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_PROPAGATION_H
