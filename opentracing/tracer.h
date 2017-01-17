#ifndef INCLUDED_OPENTRACING_TRACER_H
#define INCLUDED_OPENTRACING_TRACER_H

// ========
// tracer.h
// ========
// class GenericTracer - CRTP interface for the Tracer

#include <opentracing/carriers.h>
#include <opentracing/span.h>
#include <opentracing/spanoptions.h>
#include <opentracing/stringref.h>

namespace opentracing {

// ===================
// class GenericTracer
// ===================
// The GenericTracer acts as a factory for the other OpenTracing types. It has
// the most functionality, and also requires the most details to be instantiated
// appropriately at compile time. As such, it has the most template parameters,
// giving it access to otherwise unknowable details, at compile time.
//
// The template parameters are:
//   * TRACER  - A Tracer implementation
//   * SPAN    - A Span implementation
//   * OPTIONS - A SpanOptions implementation
//   * CONTEXT - A SpanContext implementation
//   * ADAPTER - A Baggage iterator adapter
//
// The Tracer also exposes a set of typedefs which obfuscate the details of
// the implementation. These typedefs make it possible for clients to
// rely on the installed implementation, without having to know about any of the
// underlying types.
//
//   * Span        - Aliasing GenericSpan<SPAN, CONTEXT, ADAPTER>
//   * SpanContext - Aliasing GenericSpanContex<CONTEXT, ADAPTER>
//   * SpanOptions - Aliasing GenericSpanOptions<OPTIONS, CONTEXT, ADAPTER>
//
// CRTP TRACER implementations must support the following:
//
// class TracerImp :
//  GenericTracer<TracerImp, SpanImp, OptionsImp, ContextImp, Adapter>
// {
//   public:
//     static void installImp(TracerImp*);
//     static TracerImp * instanceImp();
//     static void uninstallImp();
//
//     Options * makeSpanOptionsImp();
//     void cleanupImp(const Options * const opts);
//
//     SpanImp * start(const StringRef& op);
//     SpanImp * start(const OptionsImp& opts);
//     void cleanupImp(const SpanImp * const sp);
//
//     template<typename CARRIER>
//     int injectImp(CARRIER*const carrier, const SpanImp& context) const;
//
//     template<typename CARRIER>
//     int injectImp(CARRIER*const carrier, const ContextImp& context) const;
//
//     template <typename CARRIER>
//     const ContextImp* extractImp(const CARRIER& carrier);
//
//     void cleanupImp(const ContextImp * const);
// };

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
class GenericTracer {
  public:
    typedef GenericSpan<SPAN, CONTEXT, ADAPTER> Span;
    typedef GenericSpanContext<CONTEXT, ADAPTER> SpanContext;
    typedef GenericSpanOptions<OPTIONS, CONTEXT, ADAPTER> SpanOptions;
    // Public typedefs used by clients to use underlying implementation
    // interfaces reliably. These must exist in order to support O(1) compile
    // time changes of the Tracer implementations.

    static void install(GenericTracer* const tracer);
    // Install Tracer to be referenced globally with calls to 'instance()'.
    // It is undefined behavior to call any other Tracer method if an
    // implementation has not been installed.
    //
    // This method is not thread safe. It should be called once, in main, before
    // any other thread or trace work is performed. Undefined behavior if called
    // differently.

    static void uninstall();
    // Uninstall a previously installed Tracer. This method is not thread safe,
    // nor is it required for production code. It is used to sanitize test
    // environments when needed.

    static GenericTracer* instance();
    // Return an instance to the globally installed tracer. Returns NULL if a
    // tracer has not been previously installed. It is undefined behavior
    // to call this method without previously calling 'install()'.

    static SpanOptions* makeSpanOptions();
    // This is a factory method to create SpanOptions. Options can be passed
    // to 'start()' to control how Spans are created. Options should be
    // subsequently cleaned up with a call to 'cleanup()' Returns NULL on
    // failure.

    static void cleanup(SpanOptions* const opts);
    // All SpanOptions pointers returned by the Tracer via 'makeSpanOptions()'
    // must be passed back to the Tracer when client's are done with them via
    // 'cleanup()'.

    static Span* start(const StringRef& op);
    static Span* start(const SpanOptions& opts);
    // The 'start()' functions are factory methods to create a new Span. The
    // only required parameter is the operation string 'op', which should
    // describe the work being performed during the span.
    //
    // Optional arguments are encapsulated in the 'opts' argument. Those options
    // can be used to modify the start time, set the operation name, and/or
    // establish the causal relationship between other Spans.
    //
    // Returns NULL on error.

    static void cleanup(Span* const sp);
    // All Span pointers returned by the Tracer via 'start()' must be passed
    // back to the Tracer when clients are done with them via 'cleanup()'.

    template <typename CARRIER>
    static int inject(CARRIER* const carrier, const Span& span);
    // Inject the supplied 'span' into the supplied 'carrier' writer. This
    // method should expect one of the 'Writer' carriers defined in
    // carriers.h. Returns 0 upon success and a non-zero value otherwise.

    template <typename CARRIER>
    static int inject(CARRIER* const carrier, const SpanContext& context);
    // Inject the supplied 'span' into the supplied 'carrier' writer. This
    // method should expect one of the 'Writer' carriers defined in
    // carriers.h. Returns 0 upon success and a non-zero value otherwise.

    template <typename CARRIER>
    static const SpanContext* extract(const CARRIER& carrier);
    // Extract the read-only supplied 'context' from the text map reader
    // 'carrier' upon success, and NULL otherwise. Should expect one
    // of the 'Reader' carriers defined in carriers.h.

    static void cleanup(const SpanContext* const sp);
    // All SpanContext pointers created by the Tracer through 'extract()' should
    // be passed back to the Tracer when client's are done with them via
    // 'cleanup()'.

  protected:
    GenericTracer();
    GenericTracer(const GenericTracer&);
    // Protected to avoid direct construction
};

// -------------------
// class GenericTracer
// -------------------

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline void
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::install(
    GenericTracer* const tracer)
{
    TRACER * const tracerImp = static_cast<TRACER* const>(tracer);
    TRACER::installImp(tracerImp);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline void
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::uninstall()
{
    return TRACER::uninstallImp();
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>*
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::instance()
{
    return TRACER::instanceImp();
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::GenericTracer()
{
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::GenericTracer(
    const GenericTracer&)
{
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline typename GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::SpanOptions*
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::makeSpanOptions()
{
    return TRACER::instanceImp()->makeSpanOptionsImp();
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline void
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::cleanup(
    SpanOptions* const opts)
{
    OPTIONS* const optsImp = static_cast<OPTIONS* const>(opts);
    return TRACER::instanceImp()->cleanupImp(optsImp);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline typename GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::Span*
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::start(const StringRef& op)
{
    return TRACER::instanceImp()->startImp(op);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline typename GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::Span*
    GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::start(const SpanOptions& opts)
{
    const OPTIONS& optsImp = static_cast<const OPTIONS&>(opts);
    return TRACER::instanceImp()->startImp(optsImp);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline void
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::cleanup(Span* const sp)
{
    SPAN* const spanImp = static_cast<SPAN*>(sp);
    return TRACER::instanceImp()->cleanupImp(spanImp);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
template <typename CARRIER>
inline int
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::inject(
    CARRIER* const carrier, const Span& span)
{
    const SPAN&   spanImp = static_cast<const SPAN&>(span);
    const TRACER* tracer  = TRACER::instanceImp();
    return tracer->injectImp(carrier, spanImp);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
template <typename CARRIER>
inline int
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::inject(
    CARRIER* const carrier, const SpanContext& context)
{
    const CONTEXT& contextImp = static_cast<const CONTEXT&>(context);
    const TRACER*  tracer     = TRACER::instanceImp();

    return tracer->injectImp(carrier, contextImp);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
template <typename CARRIER>
inline const typename GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::
    SpanContext*
    GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::extract(
        const CARRIER& carrier)
{
    return TRACER::instanceImp()->extractImp(carrier);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline void
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::cleanup(
    const SpanContext* const spc)
{
    const CONTEXT* const contextImp = static_cast<const CONTEXT* const>(spc);
    return TRACER::instanceImp()->cleanupImp(contextImp);
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_TRACER_H
