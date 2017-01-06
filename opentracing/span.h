#ifndef INCLUDED_OPENTRACING_SPAN_H
#define INCLUDED_OPENTRACING_SPAN_H

// ======
// span.h
// ======
//
// class GenericSpan - CRTP interface for Spans
//
// ----
// Span
// ----
// Every Trace is made up of one or more Spans. Spans are used encapuslate:
//   * An operation name
//   * A start timestamp
//   * A finish timestamp
//   * A set of zero or more key:value SpanTags
//   * A set of zero or more timestamped key:value SpanLogs
//   * A SpanContext (see spancontext.h)
//   * References to zero or more causally-related Spans
//
// The installed Tracer acts as a factory for Spans, keeping clients agnostic
// of the implementation.
//
// See the specification for more details:
// https://github.com/opentracing/specification/blob/master/specification.md#span

#include <opentracing/config.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <opentracing/spancontext.h>
#include <opentracing/stringref.h>

namespace opentracing {

// =================
// class GenericSpan
// =================
// GenericSpan is a static, polymorphic interface for interacting with
// Spans. It uses the Curiously Repeating Template Patttern (CRTP) to
// avoid unnecessary v-table hits we would encounter with traditional
// polymorphism.
//
// See this CRTP article for more details on the design pattern:
// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
//
// -------
// Clients
// -------
// GenericSpans are not created directly by clients. They are instead
// created and cleaned up through the installed Tracer interface.
//
// Clients should never need to reference the GenericSpan type explicitly.
// Instead, clients should use the Tracer::Span typedef. By using the typedef,
// clients will avoid code changes necessary if a different Tracer
// implementation is installed later.
//
// Once clients have access to a Span, they can:
//   * Access the underlying SpanContext via 'context()'
//   * 'log()' timestamped key:value pairs
//   * 'tag()' a span, with key:value pairs
//   * 'finish()' a span
//
// After 'finish()' is called, it is undefined behavior to invoke any other
// method on the Span, except for accessing 'context()' in a read-only fashion.
// When clients are finished with a Span, it should be passed back to the
// Tracer via 'Tracer::cleanup()'.
//
// For more details on:
//   * Accessing/retrieving Baggage, see 'spancontext.h'
//   * Creating/Cleaning up Spans, see 'tracer.h'
//
// ------------
// Implementors
// ------------
// The GenericSpan has three template parameters:
//   * SPAN    - A Span implementation derived class (CRTP)
//   * CONTEXT - A SpanContext implementation derived class (CRTP)
//   * ADAPTER - A Baggage iterator adapter
//
// The CONTEXT and ADAPTER are only used to establish the GenericSpanContext
// interface used for the SpanContext typedef.
//
// SPAN implementations have to support the following:
//
// class SpanImpl: GenericSpan<SpanImpl, ContextImpl, Adapter>
// {
//   public:
//      SpanContext& contextImp();
//      const SpanContext& contextImp() const;
//
//      int finishImp();
//      int finishImp(const uint64_t);
//
//      int tagImp(const StringRef&, const Fundamental);
//      int tagImp(const StringRef&, const StringRef&);
//
//      int logImp(const StringRef&, const Fundamental);
//      int logImp(const StringRef&, const StringRef&);
//
//      int logImp(const StringRef&, const Fundamental, const uint64_t);
//      int logImp(const StringRef&, const StringRef&, const uint64_t);
// };
//
// For brevities sake, the above example collapses the overloads on fundamental
// C++ types for 'tagImp()' and 'logImp()' into a single declaration on the
// 'Fundamental' type. For actual implementations, overloads must be supplied
// for each of the following fundamental types.
//
//      * float        * int16_t  * uint16_t  * char          * wchar_t
//      * double       * int32_t  * uint32_t  * signed char   * bool
//      * long double  * int64_t  * uint64_t  * unsigned char
//      (http://en.cppreference.com/w/cpp/language/types)
//
// The overloads are used instead of a single template to:
//      * Avoid passing references larger than the types they refer to (64-bit)
//      * Avoid ambiguities at compile time when clients use the overloads

template <typename SPAN, typename CONTEXT, typename ADAPTER>
class GenericSpan {
  public:
    typedef GenericSpanContext<CONTEXT, ADAPTER> SpanContext;

    SpanContext&       context();
    const SpanContext& context() const;
    // Return the SpanContext associated with this span. Baggage can
    // be retrieved/modified through the SpanContext.

    int finish();
    int finish(const uint64_t tsp);
    // This should be the last method called on the Span. It marks the end of
    // this Span (in microseconds since epoch) using the current wall-time, or
    // an explicit timestamp 'tsp'. It is undefined behavior to call any method
    // on this span after 'finish' is called, except for 'context()'.
    // Returns 0 upon success and a non-zero value otherwise.

    int tag(const StringRef& key, const StringRef& val);
    int tag(const StringRef& key, const bool val);

    int tag(const StringRef& key, const float val);
    int tag(const StringRef& key, const double val);
    int tag(const StringRef& key, const long double val);

    int tag(const StringRef& key, const char val);
    int tag(const StringRef& key, const signed char val);
    int tag(const StringRef& key, const unsigned char val);
    int tag(const StringRef& key, const wchar_t val);

    int tag(const StringRef& key, const int16_t val);
    int tag(const StringRef& key, const int32_t val);
    int tag(const StringRef& key, const int64_t val);

    int tag(const StringRef& key, const uint16_t val);
    int tag(const StringRef& key, const uint32_t val);
    int tag(const StringRef& key, const uint64_t val);
    // Set a tag on this span, associating the supplied 'key' to the supplied
    // 'val'. Overloads are supplied for common primitive types and a string
    // blob. Returns 0 upon success and a non-zero value otherwise.

    int log(const StringRef& key, const StringRef& val);
    int log(const StringRef& key, const bool val);

    int log(const StringRef& key, const float val);
    int log(const StringRef& key, const double val);
    int log(const StringRef& key, const long double val);

    int log(const StringRef& key, const char val);
    int log(const StringRef& key, const signed char val);
    int log(const StringRef& key, const unsigned char val);
    int log(const StringRef& key, const wchar_t val);

    int log(const StringRef& key, const int16_t val);
    int log(const StringRef& key, const int32_t val);
    int log(const StringRef& key, const int64_t val);

    int log(const StringRef& key, const uint16_t val);
    int log(const StringRef& key, const uint32_t val);
    int log(const StringRef& key, const uint64_t val);
    // Log structured data for this span, associating the current wall-time
    // with the supplied key:val pair. Returns 0 upon success and a non-zero
    // value otherwise.

    int log(const StringRef& key, const StringRef& val, const uint64_t tsp);
    int log(const StringRef& key, const bool val, const uint64_t tsp);

    int log(const StringRef& key, const float val, const uint64_t tsp);
    int log(const StringRef& key, const double val, const uint64_t tsp);
    int log(const StringRef& key, const long double val, const uint64_t tsp);

    int log(const StringRef& key, const char val, const uint64_t tsp);
    int log(const StringRef& key, const signed char val, const uint64_t tsp);
    int log(const StringRef& key, const unsigned char val, const uint64_t tsp);
    int log(const StringRef& key, const wchar_t val, const uint64_t tsp);

    int log(const StringRef& key, const int16_t val, const uint64_t tsp);
    int log(const StringRef& key, const int32_t val, const uint64_t tsp);
    int log(const StringRef& key, const int64_t val, const uint64_t tsp);

    int log(const StringRef& key, const uint16_t val, const uint64_t tsp);
    int log(const StringRef& key, const uint32_t val, const uint64_t tsp);
    int log(const StringRef& key, const uint64_t val, const uint64_t tsp);
    // Log structured data for this span, associating the explicit timestamp
    // 'tsp' with the supplied 'key':'val' pair. Timestamp should be the
    // number of microseconds since the Unix Epoch. Returns 0 upon success and a
    // non-zero value otherwise.

  protected:
    GenericSpan();
    GenericSpan(const GenericSpan&);
    // Protected to avoid direct construction
};

// Protected Constructors

template <typename SPAN, typename CONTEXT, typename ADAPTER>
GenericSpan<SPAN, CONTEXT, ADAPTER>::GenericSpan()
{
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
GenericSpan<SPAN, CONTEXT, ADAPTER>::GenericSpan(const GenericSpan&)
{
}

// Context Accessors

template <typename SPAN, typename CONTEXT, typename ADAPTER>
typename GenericSpan<SPAN, CONTEXT, ADAPTER>::SpanContext&
GenericSpan<SPAN, CONTEXT, ADAPTER>::context()
{
    return static_cast<SPAN*>(this)->contextImp();
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
const typename GenericSpan<SPAN, CONTEXT, ADAPTER>::SpanContext&
GenericSpan<SPAN, CONTEXT, ADAPTER>::context() const
{
    return static_cast<const SPAN*>(this)->contextImp();
}

// Tags

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::finish()
{
    return static_cast<SPAN*>(this)->finishImp();
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::finish(const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->finishImp(tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const StringRef& val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key, const bool val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key, const float val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key, const double val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const long double val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key, const char val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const signed char val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const unsigned char val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const wchar_t val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const int16_t val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const int32_t val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const int64_t val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const uint16_t val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const uint32_t val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const uint64_t val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

// Logs

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const StringRef& val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key, const bool val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key, const float val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key, const double val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const long double val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key, const char val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const signed char val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const unsigned char val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const wchar_t val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int16_t val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int32_t val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int64_t val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint16_t val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint32_t val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint64_t val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

// Logs with timestamps
template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const StringRef& val,
                                         const uint64_t   tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const bool     val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const float    val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const double   val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const long double val,
                                         const uint64_t    tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const char     val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const signed char val,
                                         const uint64_t    tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const unsigned char val,
                                         const uint64_t      tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const wchar_t  val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int16_t  val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int32_t  val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int64_t  val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint16_t val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint32_t val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint64_t val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

}  // end namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPAN_H
