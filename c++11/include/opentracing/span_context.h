#ifndef OPENTRACING_SPAN_CONTEXT_H
#define OPENTRACING_SPAN_CONTEXT_H

namespace opentracing {
// SpanContext represents Span state that must propagate to descendant Spans and
// across process boundaries (e.g., a <trace_id, span_id, sampled> tuple).
class SpanContext {
 public:
   virtual ~SpanContext() = default;
};
} // namespace opentracing

#endif // OPENTRACING_SPAN_CONTEXT_H
