#ifndef INCLUDED_OPENTRACING_TRACER_H
#define INCLUDED_OPENTRACING_TRACER_H

#include <opentracing/config.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <opentracing/carriers.h>
#include <opentracing/span.h>
#include <opentracing/stringref.h>

namespace opentracing {

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
class GenericTracer {
  public:
    typedef GenericSpan<SPAN, CONTEXT, ADAPTER> Span;
    typedef GenericSpanContext<CONTEXT, ADAPTER> SpanContext;

    static void install(GenericTracer* const tracer);
    // Install Tracer to be referenced globally with calls to 'instance()'.
    // This method is not thread safe. It should be called once, in main, before
    // any other thread or trace work is performed. Undefined behavior if called
    // otherwise.

    static void uninstall();
    // Uninstall a previously installed Tracer. This method is not thread safe,
    // nor is it required for production code. It is used to sanitize test
    // environments if needed.

    static GenericTracer* instance();
    // Return an instance to the globally installed tracer. Returns NULL if a
    // tracer has not been previously installed. It is undefined behavior
    // to call this method without previously calling 'install()'.

    Span* start(const StringRef& op);
    Span* start(const StringRef& op, const SpanContext& parent);

    template <typename ITER>
    Span* start(const StringRef& op, const ITER& pbegin, const ITER& pend);

    Span* start(const StringRef& op, const uint64_t tsp);
    Span* start(const StringRef&   op,
                const SpanContext& parent,
                const uint64_t     tsp);

    template <typename ITER>
    Span* start(const StringRef& op,
                const ITER&      pbegin,
                const ITER&      pend,
                const uint64_t   tsp);
    // The 'start' methods are used to create a new Span. The only required
    // parameter is the operation string 'op', which should describe the work
    // being performed during the span.
    //
    // The optional arguments are:
    //   parent:
    //      Establish the new Span as a child of the supplied parent
    //
    //   pbegin/pend:
    //      Supply a range of iterators, which when dereferenced refer to
    //      a SpanContext&. The new span would be a child of all of the
    //      supplied parents Spans.
    //
    //   tsp:
    //      The time-stamp marking the start of the new Span. 'tsp' should
    //      represent the number of microseconds that have elapsed since
    //      the Unix epoch.

    void cleanup(Span *const sp);
    // All Span pointers returned by the Tracer should be passed back
    // to the Tracer when client's are done with them via 'cleanup()'.

    template <typename CARRIER>
    int inject(GenericTextWriter<CARRIER>* const carrier,
               const SpanContext&              context) const;
    // Inject the supplied 'context' into the text map writer 'carrier'.

    template <typename CARRIER>
    int inject(GenericBinaryWriter<CARRIER>* const carrier,
               const SpanContext&                context) const;
    // Inject the supplied 'context' into the binary writer 'carrier'.

    template <typename CARRIER>
    int inject(GenericWriter<CARRIER, CONTEXT>* const carrier,
               const SpanContext& context) const;
    // Inject the SpanContext directly into the specialized 'carrier'.
    //
    // Note: The carrier will be passed the SpanContext directly.
    // Using this may make your carrier code more efficient, but, it
    // removes flexibility if you want to swap Tracer/Span
    // implementations.

    template <typename CARRIER>
    SpanContext* extract(const GenericTextReader<CARRIER>& carrier);
    // Extract the supplied 'context' from the text map reader 'carrier'.

    template <typename CARRIER>
    SpanContext* extract(const GenericBinaryReader<CARRIER>& carrier);
    // Extract the supplied 'context' from the binary reader 'carrier'.

    template <typename CARRIER>
    SpanContext* extract(const GenericReader<CARRIER, CONTEXT>& carrier);
    // Extract the SpanContext from the specialized 'carrier'.
    //
    // Note: The carrier will be passed the SpanContext directly.
    // Using this may make your carrier code more efficient, but, it
    // removes flexibility if you want to swap Tracer/Span implementations
    // later.

    void cleanup(SpanContext* const sp);
    // All SpanContext pointers returned by the Tracer should be passed back
    // to the Tracer when client's are done with them via 'cleanup()'.

  protected:
    GenericTracer();
    GenericTracer(const GenericTracer&);
    // Protected to avoid direct construction

  private:
    static GenericTracer* s_tracer;
};

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>*
    GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::s_tracer = 0;

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
void
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::install(
    GenericTracer* const tracer)
{
    s_tracer = tracer;
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
void
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::uninstall()
{
    s_tracer = 0;
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>*
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::instance()
{
    return s_tracer;
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::GenericTracer()
{
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::GenericTracer(const GenericTracer&)
{
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
void
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::cleanup(Span *const sp)
{
    return static_cast<TRACER*>(this)->cleanupImp(sp);
}
template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
void
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::cleanup(SpanContext *const spc)
{
    return static_cast<TRACER*>(this)->cleanupImp(spc);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
typename GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op)
{
    return static_cast<TRACER*>(this)->startImp(op);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
typename GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op,
                                                   const uint64_t tsp)
{
    return static_cast<TRACER*>(this)->startImp(op, tsp);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
typename GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op,
                                                   const SpanContext& parent)
{
    return static_cast<TRACER*>(this)->startImp(op, parent);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
typename GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op,
                                                   const SpanContext& parent,
                                                   const uint64_t     tsp)
{
    return static_cast<TRACER*>(this)->startImp(op, parent, tsp);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename ITER>
typename GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op,
                                                   const ITER& begin,
                                                   const ITER& end)
{
    return static_cast<TRACER*>(this)->startImp(op, begin, end);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename ITER>
typename GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op,
                                                   const ITER&    begin,
                                                   const ITER&    end,
                                                   const uint64_t tsp)
{
    return static_cast<TRACER*>(this)->startImp(op, begin, end, tsp);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CARRIER>
int
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::inject(
    GenericTextWriter<CARRIER>* const carrier, const SpanContext& context) const
{
    return static_cast<const TRACER*>(this)->injectImp(carrier, context);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CARRIER>
typename GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::SpanContext*
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::extract(
    const GenericTextReader<CARRIER>& carrier)
{
    return static_cast<TRACER*>(this)->extractImp(carrier);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CARRIER>
int
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::inject(
    GenericBinaryWriter<CARRIER>* const carrier, const SpanContext& context) const
{
    return static_cast<const TRACER*>(this)->injectImp(carrier, context);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CARRIER>
typename GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::SpanContext*
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::extract(
    const GenericBinaryReader<CARRIER>& carrier)
{
    return static_cast<TRACER*>(this)->extractImp(carrier);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CARRIER>
int
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::inject(
    GenericWriter<CARRIER, CONTEXT>* const carrier,
    const SpanContext& context) const
{
    return static_cast<const TRACER*>(this)->injectImp(carrier, context);
}

template <typename TRACER, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CARRIER>
typename GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::SpanContext*
GenericTracer<TRACER, SPAN, CONTEXT, ADAPTER>::extract(
    const GenericReader<CARRIER, CONTEXT>& carrier)
{
    return static_cast<TRACER*>(this)->extractImp(carrier);
}
}
#endif
