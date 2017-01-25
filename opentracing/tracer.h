#ifndef INCLUDED_OPENTRACING_TRACER_H
#define INCLUDED_OPENTRACING_TRACER_H

#include <opentracing/carriers.h>
#include <opentracing/span.h>
#include <opentracing/spanoptions.h>
#include <opentracing/stringref.h>

namespace opentracing {

class Tracer {
  private:
    static Tracer* s_tracer;

  public:
    virtual SpanOptions* makeSpanOptions() = 0;

    virtual Span* start(const StringRef& op) = 0;
    virtual Span* start(const SpanOptions& opts) = 0;

    virtual const SpanContext* getContext(const Span*) = 0;

    virtual int inject(Writer* carrier, const Span& span) const = 0;
    virtual int inject(Writer* carrier, const SpanContext& context) const = 0;

    virtual const SpanContext* extract(const Carrier& carrier) const = 0;

    virtual void cleanup(const SpanOptions* opts) = 0;
    virtual void cleanup(const Span* sp) = 0;
    virtual void cleanup(const SpanContext* sp) = 0;

  protected:
    Tracer();
    Tracer(const Tracer&);
    // Avoid direct construction
};

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_TRACER_H
