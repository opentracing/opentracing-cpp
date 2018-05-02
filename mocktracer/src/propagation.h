#ifndef OPENTRACING_MOCKTRACER_PROPAGATION_H
#define OPENTRACING_MOCKTRACER_PROPAGATION_H

#include <opentracing/mocktracer/recorder.h>
#include <opentracing/mocktracer/tracer.h>
#include <opentracing/propagation.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

expected<void> InjectSpanContext(const PropagationOptions& propagation_options,
                                 std::ostream& carrier,
                                 const SpanContextData& span_context_data);

expected<bool> ExtractSpanContext(const PropagationOptions& propagation_options,
                                  std::istream& carrier,
                                  SpanContextData& span_context_data);

expected<void> InjectSpanContext(const PropagationOptions& propagation_options,
                                 const TextMapWriter& carrier,
                                 const SpanContextData& span_context_data);

expected<bool> ExtractSpanContext(const PropagationOptions& propagation_options,
                                  const TextMapReader& carrier,
                                  SpanContextData& span_context_data);

expected<void> InjectSpanContext(const PropagationOptions& propagation_options,
                                 const HTTPHeadersWriter& carrier,
                                 const SpanContextData& span_context_data);

expected<bool> ExtractSpanContext(const PropagationOptions& propagation_options,
                                  const HTTPHeadersReader& carrier,
                                  SpanContextData& span_context_data);
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_PROPAGATION_H
