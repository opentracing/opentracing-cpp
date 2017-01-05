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

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
class GenericTracer {
  public:
    typedef GenericSpan<SPAN, CONTEXT, ADAPTER> Span;
    typedef GenericSpanContext<CONTEXT, ADAPTER> SpanContext;

    static void install(GenericTracer* const tracer);
    // Install 'tracer' to be referenced globally. This method is not thread
    // safe.
    // It should be called once, in main, before any other thread or trace work
    // is performed. Undefined behavior otherwise.

    static void uninstall();
    // Uninstall any 'tracer' that may have been previously installed. This
    // method
    // is not thread safe. It should only be called in test environments.

    static GenericTracer* instance();
    // Return an instance to the globally installed tracer. Returns null if a
    // tracer has not been previously installed. It is undefined behavior
    // to call this method without previously calling 'install()'.

    virtual ~GenericTracer();

    Span* start(const StringRef& op);
    Span* start(const StringRef& op, const uint64_t tsp);

    Span* start(const StringRef& op, const SpanContext& parent);

    Span* start(const StringRef&   op,
                const SpanContext& parent,
                const uint64_t     tsp);

    template <typename ITER>
    Span* start(const StringRef& op, const ITER& pbegin, const ITER& pend);

    template <typename ITER>
    Span* start(const StringRef& op,
                const ITER&      pbegin,
                const ITER&      pend,
                const uint64_t   tsp);
    // The 'start' methods are used to create a new 'Span'. The only required
    // parameter is the operation string 'op', which should describe the work
    // being performed during the span.
    //
    // The optional arguments are:
    //   parent:
    //      Refer a SpanContext, establishing the new span as a child of the
    //      supplied context
    //
    //   pbegin/pend:
    //      Supply a range of iterators, which when dereferenced refer to
    //      a SpanContext&. The new span would be a child of all of the
    //      supplied parents.
    //
    //   tsp:
    //      The time-stamp marking the start of the new Span. 'tsp' should
    //      represent the number of microseconds that have elapsed since
    //      the Unix epoch.

    template <typename CIMPL>
    int inject(GenericTextWriter<CIMPL>* const carrier,
               const SpanContext&              context) const;
    // Inject the supplied 'context' into the text map writer 'carrier'.

    template <typename CIMPL>
    SpanContext* extract(const GenericTextReader<CIMPL>& carrier);
    // Extract the supplied 'context' from the text map reader 'carrier'.

    template <typename CIMPL>
    int inject(GenericBinaryWriter<CIMPL>* const carrier,
               const SpanContext&                context) const;
    // Inject the supplied 'context' into the binary writer 'carrier'.

    template <typename CIMPL>
    SpanContext* extract(const GenericBinaryReader<CIMPL>& carrier);
    // Extract the supplied 'context' from the binary reader 'carrier'.

    template <typename CIMPL>
    int inject(GenericWriter<CIMPL, CONTEXT>* const carrier,
               const SpanContext& context) const;
    // Inject the SpanContext directly into the specialized 'carrier'.
    //
    // Note: The carrier will be passed the 'SpanContext' directly.
    // Using this may make your carrier code more efficient, but it
    // removes flexibility if you want to swap Tracer/Span
    // implementations.

    template <typename CIMPL>
    SpanContext* extract(const GenericReader<CIMPL, CONTEXT>& carrier);
    // Extract the SpanContext from the specialized 'carrier'.
    //
    // Note: The carrier will be passed the 'SpanContext' directly.
    // Using this may make your carrier code more efficient, but it
    // removes flexibility if you want to swap Tracer/Span implementations
    // later.

  protected:
    GenericTracer();
    GenericTracer(const GenericTracer&);
    // Protected to avoid direct construction

  private:
    static GenericTracer* s_tracer;
};

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>*
    GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::s_tracer = 0;

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
void
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::install(
    GenericTracer* const tracer)
{
    s_tracer = tracer;
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
void
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::uninstall()
{
    s_tracer = 0;
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>*
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::instance()
{
    return s_tracer;
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::~GenericTracer()
{
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::GenericTracer()
{
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::GenericTracer(const GenericTracer&)
{
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
typename GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op)
{
    return static_cast<IMPL*>(this)->startImp(op);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
typename GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op,
                                                   const uint64_t tsp)
{
    return static_cast<IMPL*>(this)->startImp(op, tsp);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
typename GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op,
                                                   const SpanContext& parent)
{
    return static_cast<IMPL*>(this)->startImp(op, parent);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
typename GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op,
                                                   const SpanContext& parent,
                                                   const uint64_t     tsp)
{
    return static_cast<IMPL*>(this)->startImp(op, parent, tsp);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename ITER>
typename GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op,
                                                   const ITER& begin,
                                                   const ITER& end)
{
    return static_cast<IMPL*>(this)->startImp(op, begin, end);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename ITER>
typename GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::Span*
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::start(const StringRef& op,
                                                   const ITER&    begin,
                                                   const ITER&    end,
                                                   const uint64_t tsp)
{
    return static_cast<IMPL*>(this)->startImp(op, begin, end, tsp);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CIMPL>
int
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::inject(
    GenericTextWriter<CIMPL>* const carrier, const SpanContext& context) const
{
    return static_cast<const IMPL*>(this)->injectImp(carrier, context);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CIMPL>
typename GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::SpanContext*
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::extract(
    const GenericTextReader<CIMPL>& carrier)
{
    return static_cast<IMPL*>(this)->extractImp(carrier);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CIMPL>
int
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::inject(
    GenericBinaryWriter<CIMPL>* const carrier, const SpanContext& context) const
{
    return static_cast<const IMPL*>(this)->injectImp(carrier, context);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CIMPL>
typename GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::SpanContext*
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::extract(
    const GenericBinaryReader<CIMPL>& carrier)
{
    return static_cast<IMPL*>(this)->extractImp(carrier);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CIMPL>
int
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::inject(
    GenericWriter<CIMPL, CONTEXT>* const carrier,
    const SpanContext& context) const
{
    return static_cast<const IMPL*>(this)->injectImp(carrier, context);
}

template <typename IMPL, typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename CIMPL>
typename GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::SpanContext*
GenericTracer<IMPL, SPAN, CONTEXT, ADAPTER>::extract(
    const GenericReader<CIMPL, CONTEXT>& carrier)
{
    return static_cast<IMPL*>(this)->extractImp(carrier);
}
}
#endif
