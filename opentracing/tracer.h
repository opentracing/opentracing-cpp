#ifndef INCLUDED_OPENTRACING_TRACER_H
#define INCLUDED_OPENTRACING_TRACER_H

#include <opentracing/config.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <opentracing/carriers.h>
#include <opentracing/stringref.h>

namespace opentracing {

template <typename IMPL>
class GenericTracer {
    typedef IMPL              Tracer;
    typedef Tracer::Span      Span;
    typedef Span::SpanContext SpanContext;

    int start(Span* const sp, const StringRef& operation);
    int start(Span* const sp, const StringRef& operation, const uint64_t tsp);

    int start(Span* const        sp,
              const StringRef&   operation,
              const SpanContext& parent);

    int start(Span* const        sp,
              const StringRef&   operation,
              const SpanContext& parent,
              const uint64_t     tsp);

    template <typename ITER>
    int start(Span* const      sp,
              const StringRef& operation,
              const ITER       begin,
              const ITER       end);

    template <typename ITER>
    int start(Span* const      sp,
              const StringRef& operation,
              const ITER       begin,
              const ITER       end,
              const uint64_t   tsp);
    // The 'start' methods are used to load a new 'Span'. The only required
    // parameter is the 'operation' description, which should outline the
    // work being performed during the Span.
    //
    // The optional arguments are:
    //   parent:
    //      Reference a single parent Span's context, establishing this
    //      Span's trace id
    //
    //   begin/end:
    //      A set of iterators that outline a range of parent Span contexts.
    //      This Span should be marked as a child of all the supplied
    //      parents. ITER must be a forward iterator, that when dereferenced,
    //      can be stored in a `const SpanContext&`.
    //
    //   tsp:
    //      The time-stamp marking the start of the Span, in microseconds.
    //      If not supplied, the current wall-time should be used.

    template <typename CIMPL>
    int inject(GenericTextWriter<CIMPL>* const carrier,
               const SpanContext&              context) const;
    // Inject the supplied 'context' into the text map writer 'carrier'.

    template <typename CIMPL>
    int extract(SpanContext* const              context,
                const GenericTextReader<CIMPL>& carrier);
    // Extract the supplied 'context' from the text map reader 'carrier'.

    template <typename CIMPL>
    int inject(GenericBinaryWriter<CIMPL>* const carrier,
               const SpanContext&                context) const;
    // Inject the supplied 'context' into the binary writer 'carrier'.

    template <typename CIMPL>
    int extract(SpanContext* const                context,
                const GenericBinaryReader<CIMPL>& carrier);
    // Extract the supplied 'context' from the binary reader 'carrier'.

    template <typename CIMPL>
    int inject(GenericWriter<CIMPL>* const carrier,
               const SpanContext&          context) const;
    // Inject the SpanContext directly into the specialized 'carrier'.
    //
    // Note: The carrier will be passed the 'SpanContext' directly. Using
    // this may make your carrier code more efficient, but it removes
    // flexibility if you want to swap Tracer/Span implementations later.

    template <typename CIMPL>
    int extract(SpanContext* const          context,
                const GenericReader<CIMPL>& carrier);
    // Extract the SpanContext from the specialized 'carrier'.
    //
    // Note: The carrier will be passed the 'SpanContext' directly. Using
    // this may make your carrier code more efficient, but it removes
    // flexibility if you want to swap Tracer/Span implementations later.

  protected:
    GenericTracer();
    GenericTracer(const GenericTracer&);
    // Protected to avoid direct construction
};

template<typename IMPL>
GenericTracer::GenericTracer()
{
}

template <typename IMPL>
GenericTracer::GenericTracer(const GenericTracer&)
{
}

template <typename IMPL>
int
GenericTracer::start(Span* const Span)
{
    return static_cast<Tracer*>(this)->start(Span);
}

template <typename IMPL>
int
GenericTracer::start(Span* const sp, const uint64_t tsp)
{
    return static_cast<Tracer*>(this)->start(sp, tsp);
}

template <typename IMPL>
int
GenericTracer::start(Span* const sp, const SpanContext& parent)
{
    return static_cast<Tracer*>(this)->start(sp, parent);
}

template <typename IMPL>
int
GenericTracer::start(Span* const        sp,
                     const SpanContext& parent,
                     const uint64_t     tsp)
{
    return static_cast<Tracer*>(this)->start(sp, parent, tsp);
}

template <typename ITER>
int
GenericTracer::start(Span* const sp, const ITER begin, const ITER end)
{
    return static_cast<Tracer*>(this)->start(sp, begin, end);
}

template <typename ITER>
int
GenericTracer::start(Span* const    sp,
                     const ITER     begin,
                     const ITER     end,
                     const uint64_t tsp)
{
    return static_cast<Tracer*>(this)->start(sp, begin, end, tsp);
}

template <typename IMPL>
template <typename CIMPL>
int
GenericTracer::inject(GenericTextWriter<CIMPL>* const carrier,
                      const SpanContext&              context) const
{
    return static_cast<Tracer*>(this)->inject(carrier, context);
}

template <typename IMPL>
template <typename CIMPL>
int
GenericTracer::extract(SpanContext* const              context,
                       const GenericTextReader<CIMPL>& carrier)
{
    return static_cast<Tracer*>(this)->extract(context, carrier);
}

template <typename IMPL>
template <typename CIMPL>
int
GenericTracer::inject(GenericBinaryWriter<CIMPL>* const carrier,
                      const SpanContext&                context) const
{
    return static_cast<Tracer*>(this)->inject(carrier, context);
}

template <typename IMPL>
template <typename CIMPL>
int
GenericTracer::extract(SpanContext* const                context,
                       const GenericBinaryReader<CIMPL>& carrier)
{
    return static_cast<Tracer*>(this)->extract(context, carrier);
}

template <typename IMPL>
template <typename CIMPL>
int
GenericTracer::inject(GenericWriter<CIMPL>* const carrier,
                      const SpanContext&          context) const
{
    return static_cast<Tracer*>(this)->inject(carrier, context);
}

template <typename IMPL>
template <typename CIMPL>
int
GenericTracer::extract(SpanContext* const          context,
                       const GenericReader<CIMPL>& carrier)
{
    return static_cast<Tracer*>(this)->extract(context, carrier);
}
}
#endif
