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
#include <opentracing/tracerguards.h>

#include <opentracing/config.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

namespace opentracing {

// ===================
// class GenericTracer
// ===================
// GenericTracer is a static, polymorphic interface for interacting with
// the installed Tracer implementation. It uses the Curiously Repeating Template
// Pattern (CRTP) to avoid v-table hits we would encounter with traditional
// polymorphism.
//
// See this CRTP article for more details on the design pattern:
// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
//
// The GenericTracer acts as a factory for the other OpenTracing types. It has
// the most functionality, and also requires the most details to be instantiated
// appropriately at compile time. As such, it has the most template parameters,
// giving it access to otherwise unknowable details, at compile time.
//
// The template parameters are:
//   * TRACER  - A Tracer implementation (CRTP)
//   * SPAN    - A Span implementation (CRTP)
//   * OPTIONS - A SpanOptions implementation (CRTP)
//   * CONTEXT - A SpanContext implementation (CRTP)
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
// TRACER implementations must support the following:
//
// class TracerImp :
//  GenericTracer<TracerImp, SpanImp, OptionsImp, ContextImp, Adapter>
// {
//   public:
//     Options * makeSpanOptionsImp();
//     void cleanupImp(const Options * const opts);
//
//     SpanImp * start(const StringRef& op);
//     SpanImp * start(const OptionsImp& opts);
//     void cleanupImp(const SpanImp * const sp);
//
//     template<typename CARRIER>
//     int injectImp(
//              GenericTextWriter<CARRIER>* const carrier,
//              const ContextImp&                 context) const;
//
//     template<typename CARRIER>
//     int injectImp(
//              GenericBinaryWriter<CARRIER>* const carrier,
//              const ContextImp&                   context) const;
//
//     template <typename CARRIER>
//     int injectImp(
//              GenericWriter<CARRIER, ContextImp>* const carrier,
//              const ContextImp&                         context) const;
//
//     template <typename CARRIER>
//     const ContextImp* extractImp(
//              const GenericTextReader<CARRIER>& carrier);
//
//     template <typename CARRIER>
//     const ContextImp* extractImp(
//              const GenericBinaryReader<CARRIER>& carrier);
//
//     template <typename CARRIER>
//     const ContextImp* extractImp(
//              const GenericReader<CARRIER, ContextImp>& carrier);
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
    typedef GenericSpan<SPAN, CONTEXT, ADAPTER>                  Span;
    typedef GenericSpanContext<CONTEXT, ADAPTER>                 SpanContext;
    typedef GenericSpanOptions<OPTIONS, CONTEXT, ADAPTER>        SpanOptions;

    typedef GenericTracerGuard<GenericTracer, Span>              SpanGuard;
    typedef GenericTracerGuard<GenericTracer, SpanOptions>       SpanOptionsGuard;
    typedef GenericTracerGuard<GenericTracer, const SpanContext> SpanContextGuard;
    // Public typedefs used by clients to use underlying implementation
    // interfaces reliably. These must exist in order to support O(1) compile
    // time changes of the Tracer implementations.

    static void install(GenericTracer* const tracer);
    // Install Tracer to be referenced globally with calls to 'instance()'.
    // This method is not thread safe. It should be called once, in main, before
    // any other thread or trace work is performed. Undefined behavior if called
    // differently.

    static void uninstall();
    // Uninstall a previously installed Tracer. This method is not thread safe,
    // nor is it required for production code. It is used to sanitize test
    // environments if needed.

    static GenericTracer* instance();
    // Return an instance to the globally installed tracer. Returns NULL if a
    // tracer has not been previously installed. It is undefined behavior
    // to call this method without previously calling 'install()'.

    SpanOptions* makeSpanOptions();
    // This is a factory method to create SpanOptions. Options can be passed
    // to 'start()' to control how Spans are created. Options should be
    // subsequently cleaned up with a call to 'cleanup()' Returns NULL on
    // failure.

    void cleanup(SpanOptions* const opts);
    // All SpanOptions pointers returned by the Tracer via 'makeSpanOptions()'
    // must be passed back to the Tracer when client's are done with them via
    // 'cleanup()'.

    Span* start(const StringRef& op);
    Span* start(const SpanOptions& opts);
    // The 'start()' functions are factory methods to create a new Span. The
    // only required parameter is the operation string 'op', which should
    // describe the work being performed during the span.
    //
    // Optional arguments are encapsulated in the 'opts' argument. Those options
    // can be used to modify the start time, set the operation name, and/or
    // establish the causal relationship between other Spans.
    //
    // Returns NULL on error.

    void cleanup(Span* const sp);
    // All Span pointers returned by the Tracer via 'start()' must be passed
    // back to the Tracer when clients are done with them via 'cleanup()'.

    template <typename CARRIER>
    int inject(GenericTextWriter<CARRIER>* const carrier,
               const SpanContext&                context) const;
    // Inject the supplied 'context' into the text map writer 'carrier'.
    // Returns 0 upon success and a non-zero value otherwise.

    template <typename CARRIER>
    int inject(GenericBinaryWriter<CARRIER>* const carrier,
               const SpanContext&                  context) const;
    // Inject the supplied 'context' into the binary writer 'carrier'.
    // Returns 0 upon success and a non-zero value otherwise.

    template <typename CARRIER>
    int inject(GenericWriter<CARRIER, CONTEXT>* const carrier,
               const SpanContext& context) const;
    // Inject the SpanContext directly into the specialized 'carrier'.
    // Returns 0 upon success and a non-zero value otherwise.
    //
    // Note: The carrier will be passed the SpanContext directly. Using this may
    // make your carrier code more efficient, but, it removes flexibility if you
    // want to swap Tracer/Span implementations.

    template <typename CARRIER>
    const SpanContext* extract(const GenericTextReader<CARRIER>& carrier);
    // Extract the read-only supplied 'context' from the text map reader
    // 'carrier' upon success, and NULL otherwise. Returns NULL on error.

    template <typename CARRIER>
    const SpanContext* extract(const GenericBinaryReader<CARRIER>& carrier);
    // Extract the read-only SpanContext from the binary reader 'carrier'
    // upon success, and NULL otherwise. Returns NULL on error.

    template <typename CARRIER>
    const SpanContext* extract(const GenericReader<CARRIER, CONTEXT>& carrier);
    // Extract the read-only SpanContext from the specialized 'carrier'. Returns
    // NULL on error.
    //
    // Note: The carrier will be passed the SpanContext directly.
    // Using this may make your carrier code more efficient, but, it
    // removes flexibility if you want to swap Tracer/Span implementations
    // later.

    void cleanup(const SpanContext* const sp);
    // All SpanContext pointers created by the Tracer through 'extract()' should
    // be passed back to the Tracer when client's are done with them via
    // 'cleanup()'.

  protected:
    GenericTracer();
    GenericTracer(const GenericTracer&);
    // Protected to avoid direct construction

  private:
    static GenericTracer* s_tracer;
};

// -------------------
// class GenericTracer
// -------------------

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>*
    GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::s_tracer = 0;

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline void
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::install(
    GenericTracer* const tracer)
{
    s_tracer = tracer;
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline void
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::uninstall()
{
    s_tracer = 0;
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>*
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::instance()
{
    return s_tracer;
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
    return static_cast<TRACER*>(this)->makeSpanOptionsImp();
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
    return static_cast<TRACER*>(this)->cleanupImp(optsImp);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
inline typename GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::Span*
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::start(const StringRef& op)
{
    return static_cast<TRACER*>(this)->startImp(op);
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
    return static_cast<TRACER*>(this)->startImp(optsImp);
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
    spanImp->finish();
    return static_cast<TRACER*>(this)->cleanupImp(spanImp);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
template <typename CARRIER>
inline int
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::inject(
    GenericTextWriter<CARRIER>* const carrier, const SpanContext& context) const
{
    const CONTEXT& contextImp = static_cast<const CONTEXT&>(context);
    return static_cast<const TRACER*>(this)->injectImp(carrier, contextImp);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
template <typename CARRIER>
inline int
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::inject(
    GenericBinaryWriter<CARRIER>* const carrier,
    const SpanContext&                  context) const
{
    const CONTEXT& contextImp = static_cast<const CONTEXT&>(context);
    return static_cast<const TRACER*>(this)->injectImp(carrier, contextImp);
}

template <typename TRACER,
          typename SPAN,
          typename OPTIONS,
          typename CONTEXT,
          typename ADAPTER>
template <typename CARRIER>
inline int
GenericTracer<TRACER, SPAN, OPTIONS, CONTEXT, ADAPTER>::inject(
    GenericWriter<CARRIER, CONTEXT>* const carrier,
    const SpanContext& context) const
{
    const CONTEXT& contextImp = static_cast<const CONTEXT&>(context);
    return static_cast<const TRACER*>(this)->injectImp(carrier, contextImp);
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
        const GenericTextReader<CARRIER>& carrier)
{
    return static_cast<TRACER*>(this)->extractImp(carrier);
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
        const GenericBinaryReader<CARRIER>& carrier)
{
    return static_cast<TRACER*>(this)->extractImp(carrier);
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
        const GenericReader<CARRIER, CONTEXT>& carrier)
{
    return static_cast<TRACER*>(this)->extractImp(carrier);
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
    return static_cast<TRACER*>(this)->cleanupImp(contextImp);
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_TRACER_H
