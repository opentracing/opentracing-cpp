#ifndef INCLUDED_OPENTRACING_SPAN_H
#define INCLUDED_OPENTRACING_SPAN_H

#include <opentracing/spancontext.h>
#include <opentracing/stringref.h>

namespace opentracing {

template <typename IMPL>
class GenericSpan {
  public:
    typedef IMPL              Span;
    typedef Span::SpanContext SpanContext;

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
    int tag(const StringRef& key, const int64_t val);
    int tag(const StringRef& key, const uint64_t val);
    int tag(const StringRef& key, const double val);
    int tag(const StringRef& key, const StringRef& val);
    // Set a tag on this span, associating the supplied 'key' characters to
    // the supplied value.

    int log(const StringRef& key, const bool val);
    int log(const StringRef& key, const int64_t val);
    int log(const StringRef& key, const uint64_t val);
    int log(const StringRef& key, const double val);
    int log(const StringRef& key, const StringRef& val);

    int log(const StringRef& key, const bool val, const uint64_t tsp);
    int log(const StringRef& key, const int64_t val, const uint64_t tsp);
    int log(const StringRef& key, const uint64_t val, const uint64_t tsp);
    int log(const StringRef& key, const double val, const uint64_t tsp);
    int log(const StringRef& key, const StringRef& val, const uint64_t tsp);
    // Structured data should be logged through one of the calls to 'log'.
    // From this point forward, this span will associate 'key' with the
    // supplied 'value'. This is similar to 'tag()', but additionally includes
    // a time-stamp noting the number of microseconds that have elapsed since
    // the Unix epoch. If time-stamp is not supplied, the current wall-time
    // is used, otherwise the explicit time-stamp is used. The time-stamp
    // should be between the start and finish times for this span.

  protected:
    GenericSpan();
    GenericSpan(const GenericSpan&);
    // Protected to avoid direct construction
};

template<typename IMPL>
GenericSpan::GenericSpan()
{
}

template<typename IMPL>
GenericSpan::GenericSpan(const GenericSpan&)
{
}

template <typename IMPL>
GenericSpan::SpanContext&
GenericSpan::context()
{
    return static_cast<span*>(this)->context();
}

template <typename IMPL>
const GenericSpan::SpanContext&
GenericSpan::context() const
{
    return static_cast<span*>(this)->context();
}

template <typename IMPL>
int
GenericSpan::finish()
{
    return static_cast<span*>(this)->finish();
}

template <typename IMPL>
int
GenericSpan::finish(const uint64_t tsp)
{
    return static_cast<span*>(this)->finish(tsp);
}

template <typename IMPL>
int
GenericSpan::tag(const StringRef& key, const bool val)
{
    return static_cast<span*>(this)->tag(val);
}

template <typename IMPL>
int
GenericSpan::tag(const StringRef& key, const int64_t val)
{
    return static_cast<span*>(this)->tag(key, val);
}

template <typename IMPL>
int
GenericSpan::tag(const StringRef& key, const uint64_t val)
{
    return static_cast<span*>(this)->tag(key, val);
}

template <typename IMPL>
int
GenericSpan::tag(const StringRef& key, const double val)
{
    return static_cast<span*>(this)->tag(key, val);
}

template <typename IMPL>
int
GenericSpan::tag(const StringRef& key, const StringRef& val)
{
    return static_cast<span*>(this)->tag(key, val, vlen);
}

template <typename IMPL>
int
GenericSpan::log(const StringRef& key, const bool val)
{
    return static_cast<span*>(this)->log(key, val);
}

template <typename IMPL>
int
GenericSpan::log(const StringRef& key, const int64_t val)
{
    return static_cast<span*>(this)->log(key, val);
}

template <typename IMPL>
int
GenericSpan::log(const StringRef& key, const uint64_t val)
{
    return static_cast<span*>(this)->log(key, val);
}

template <typename IMPL>
int
GenericSpan::log(const StringRef& key, const double val)
{
    return static_cast<span*>(this)->log(key, val);
}

template <typename IMPL>
int
GenericSpan::log(const StringRef& key, const StringRef& val)
{
    return static_cast<span*>(this)->log(key, val);
}

template <typename IMPL>
int
GenericSpan::log(const StringRef& key, const bool val, const uint64_t tsp)
{
    return static_cast<span*>(this)->log(key, val, tsp);
}

template <typename IMPL>
int
GenericSpan::log(const StringRef& key, const int64_t val, const uint64_t tsp)
{
    return static_cast<span*>(this)->log(key, val, tsp);
}

template <typename IMPL>
int
GenericSpan::log(const StringRef& key, const uint64_t val, const uint64_t tsp)
{
    return static_cast<span*>(this)->log(key, val, tsp);
}

template <typename IMPL>
int
GenericSpan::log(const StringRef& key, const double val, const uint64_t tsp)
{
    return static_cast<span*>(this)->log(key, val, tsp);
}

template <typename IMPL>
int
GenericSpan::log(const StringRef& key, const StringRef& val, const uint64_t tsp)
{
    return static_cast<span*>(this)->log(key, val, tsp);
}

}  // end namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPAN_H
