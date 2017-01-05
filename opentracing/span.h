#ifndef INCLUDED_OPENTRACING_SPAN_H
#define INCLUDED_OPENTRACING_SPAN_H

#include <opentracing/config.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <opentracing/spancontext.h>
#include <opentracing/stringref.h>

namespace opentracing {

template <typename IMPL, typename CONTEXT, typename ADAPTER>
class GenericSpan {
  public:
    virtual ~GenericSpan();

    typedef IMPL Span;
    typedef GenericSpanContext<CONTEXT, ADAPTER> SpanContext;

    SpanContext&       context();
    const SpanContext& context() const;

    // Return the SpanContext associated with this span. Baggage can
    // be set or accessed through the underlying context directly.

    int finish();
    int finish(const uint64_t tsp);
    // Finish this span. Mark the end of the span (in microseconds since epoch)
    // using the 'tsp' time-stamp. If 'tsp' is not supplied, the internal
    // wall-time should be used to mark the end of the span instead. It is
    // undefined behavior to call any method on this span after 'finish' is
    // called, except for 'context()'.

    int tag(const StringRef& key, const bool val);
    int tag(const StringRef& key, const float val);
    int tag(const StringRef& key, const double val);
    int tag(const StringRef& key, const StringRef& val);

    int tag(const StringRef& key, const int16_t val);
    int tag(const StringRef& key, const int32_t val);
    int tag(const StringRef& key, const int64_t val);

    int tag(const StringRef& key, const uint16_t val);
    int tag(const StringRef& key, const uint32_t val);
    int tag(const StringRef& key, const uint64_t val);

    // Set a tag on this span, associating the supplied 'key' characters to
    // the supplied value. Overloads are supplied for common primitive types,
    // and a string blob

    int log(const StringRef& key, const bool val);
    int log(const StringRef& key, const float val);
    int log(const StringRef& key, const double val);
    int log(const StringRef& key, const StringRef& val);

    int log(const StringRef& key, const int16_t val);
    int log(const StringRef& key, const int32_t val);
    int log(const StringRef& key, const int64_t val);

    int log(const StringRef& key, const uint16_t val);
    int log(const StringRef& key, const uint32_t val);
    int log(const StringRef& key, const uint64_t val);

    int log(const StringRef& key, const bool val, const uint64_t tsp);
    int log(const StringRef& key, const float val, const uint64_t tsp);
    int log(const StringRef& key, const double val, const uint64_t tsp);
    int log(const StringRef& key, const StringRef& val, const uint64_t tsp);

    int log(const StringRef& key, const int16_t val, const uint64_t tsp);
    int log(const StringRef& key, const int32_t val, const uint64_t tsp);
    int log(const StringRef& key, const int64_t val, const uint64_t tsp);

    int log(const StringRef& key, const uint16_t val, const uint64_t tsp);
    int log(const StringRef& key, const uint32_t val, const uint64_t tsp);
    int log(const StringRef& key, const uint64_t val, const uint64_t tsp);

    // Structured data should be logged through one of the calls to 'log'. From
    // this point forward, this span will associate 'key' with the supplied
    // 'value'. This is similar to 'tag()', but additionally includes a
    // time-stamp noting the number of microseconds that have elapsed since
    // the Unix epoch. If time-stamp is not supplied, the current wall-time
    // is used, otherwise the explicit time-stamp is used. The time-stamp
    // should be between the start and finish times for this span.

  protected:
    GenericSpan();
    GenericSpan(const GenericSpan&);
    // Protected to avoid direct construction
};

// Virtual destructor

template <typename IMPL, typename CONTEXT, typename ADAPTER>
GenericSpan<IMPL, CONTEXT, ADAPTER>::~GenericSpan()
{
}

// Protected Constructors

template <typename IMPL, typename CONTEXT, typename ADAPTER>
GenericSpan<IMPL, CONTEXT, ADAPTER>::GenericSpan()
{
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
GenericSpan<IMPL, CONTEXT, ADAPTER>::GenericSpan(const GenericSpan&)
{
}

// Context Accessors

template <typename IMPL, typename CONTEXT, typename ADAPTER>
typename GenericSpan<IMPL, CONTEXT, ADAPTER>::SpanContext&
GenericSpan<IMPL, CONTEXT, ADAPTER>::context()
{
    return static_cast<Span*>(this)->contextImp();
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
const typename GenericSpan<IMPL, CONTEXT, ADAPTER>::SpanContext&
GenericSpan<IMPL, CONTEXT, ADAPTER>::context() const
{
    return static_cast<const Span*>(this)->contextImp();
}

// Tags

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::finish()
{
    return static_cast<Span*>(this)->finishImp();
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::finish(const uint64_t tsp)
{
    return static_cast<Span*>(this)->finishImp(tsp);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::tag(const StringRef& key, const bool val)
{
    return static_cast<Span*>(this)->tagImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::tag(const StringRef& key, const float val)
{
    return static_cast<Span*>(this)->tagImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::tag(const StringRef& key, const double val)
{
    return static_cast<Span*>(this)->tagImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const StringRef& val)
{
    return static_cast<Span*>(this)->tagImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const int16_t val)
{
    return static_cast<Span*>(this)->tagImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const int32_t val)
{
    return static_cast<Span*>(this)->tagImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const int64_t val)
{
    return static_cast<Span*>(this)->tagImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const uint16_t val)
{
    return static_cast<Span*>(this)->tagImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const uint32_t val)
{
    return static_cast<Span*>(this)->tagImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::tag(const StringRef& key,
                                         const uint64_t val)
{
    return static_cast<Span*>(this)->tagImp(key, val);
}

// Logs

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key, const bool val)
{
    return static_cast<Span*>(this)->logImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key, const float val)
{
    return static_cast<Span*>(this)->logImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key, const double val)
{
    return static_cast<Span*>(this)->logImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const StringRef& val)
{
    return static_cast<Span*>(this)->logImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int16_t val)
{
    return static_cast<Span*>(this)->logImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int32_t val)
{
    return static_cast<Span*>(this)->logImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int64_t val)
{
    return static_cast<Span*>(this)->logImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint16_t val)
{
    return static_cast<Span*>(this)->logImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint32_t val)
{
    return static_cast<Span*>(this)->logImp(key, val);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint64_t val)
{
    return static_cast<Span*>(this)->logImp(key, val);
}

// Logs with timestamps
//
template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const bool     val,
                                         const uint64_t tsp)
{
    return static_cast<Span*>(this)->logImp(key, val, tsp);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const float    val,
                                         const uint64_t tsp)
{
    return static_cast<Span*>(this)->logImp(key, val, tsp);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const double   val,
                                         const uint64_t tsp)
{
    return static_cast<Span*>(this)->logImp(key, val, tsp);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const StringRef& val,
                                         const uint64_t   tsp)
{
    return static_cast<Span*>(this)->logImp(key, val, tsp);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int16_t  val,
                                         const uint64_t tsp)
{
    return static_cast<Span*>(this)->logImp(key, val, tsp);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int32_t  val,
                                         const uint64_t tsp)
{
    return static_cast<Span*>(this)->logImp(key, val, tsp);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const int64_t  val,
                                         const uint64_t tsp)
{
    return static_cast<Span*>(this)->logImp(key, val, tsp);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint16_t val,
                                         const uint64_t tsp)
{
    return static_cast<Span*>(this)->logImp(key, val, tsp);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint32_t val,
                                         const uint64_t tsp)
{
    return static_cast<Span*>(this)->logImp(key, val, tsp);
}

template <typename IMPL, typename CONTEXT, typename ADAPTER>
int
GenericSpan<IMPL, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const uint64_t val,
                                         const uint64_t tsp)
{
    return static_cast<Span*>(this)->logImp(key, val, tsp);
}

}  // end namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPAN_H
