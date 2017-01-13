#ifndef INCLUDED_OPENTRACING_SPAN_H
#define INCLUDED_OPENTRACING_SPAN_H

// ======
// span.h
// ======
// class GenericSpan - CRTP interface for Spans

#include <opentracing/spancontext.h>
#include <opentracing/spanoptions.h>
#include <opentracing/stringref.h>
#include <stdint.h>

namespace opentracing {

// =================
// class GenericSpan
// =================
// The GenericSpan has three template parameters:
//   * SPAN    - A Span implementation derived class
//   * CONTEXT - A SpanContext implementation derived class
//   * ADAPTER - A Baggage iterator adapter
//
// The CONTEXT and ADAPTER are used to alias the related GenericSpanContext
// class as the SpanContext. This is a convenience typedef to ensure what is
// exposed to clients is the same as the type used by the interface.
//
// CRTP SPAN implementations must support the following:
//
// class SpanImpl: GenericSpan<SpanImpl, ContextImpl, Adapter>
// {
//   public:
//      const ContextImpl * contextImp() const;
//
//      int setOperationImp(const StringRef& operation);
//      int setBaggageImp(const StringRef&, const StringRef&);
//
//      int getBaggageImp(const StringRef&, std::string* const) const;
//      int getBaggageImp(const StringRef&, std::vector<std::string>* const) const;
//
//      int finishImp();
//      int finishImp(const uint64_t);
//
//      template<typename T>
//      int tagImp(const StringRef&, const T&);
//
//      template<typename T>
//      int logImp(const StringRef&, const T&);
//
//      template<typename T>
//      int logImp(const StringRef&, const T&, const uint64_t);
// };
//
// The templated 'tagImp' and 'logImp' methods may assume that the following
// method is defined for any type 'T':
//      `std::ostream& ::operator<<(std::ostream&, const T& val);`

template <typename SPAN, typename CONTEXT, typename ADAPTER>
class GenericSpan {
  public:
    typedef GenericSpanContext<CONTEXT, ADAPTER> SpanContext;

    const SpanContext * context() const;
    // Return a copy of the SpanContext associated with this span.
    // The context can be injected into carriers, or added as a reference to
    // other causally related Spans. When finished with the SpanContext, it
    // should be cleaned up with 'Tracer::cleanup()'.

    int setBaggage(const StringRef& key, const StringRef& value);
    // Add the 'key:value' pair to this Span. Baggage is propagated with the Span
    // in-band, throughout the lifetime of a Trace. Return 0 upon success
    // and a non-zero value otherwise.

    int getBaggage(const StringRef &key, std::string *const baggage) const;
    // Load a single 'baggage' value associated with 'key'. Returns 0 if there
    // is only one value associated with 'key' and that value was loaded
    // successfully. Return a non-zero value otherwise.

    int getBaggage(const StringRef &               key,
                   std::vector<std::string> *const baggage) const;
    // Load the 'baggage' associated with 'key'. Returns 0 if the
    // baggage is loaded successfully, and a non-zero value otherwise.

    int setOperation(const StringRef& operation);
    // Modify the Span's operation. Return 0 upon success and a non-zero
    // value otherwise. It is undefined behavior to call this method after
    // 'finish()'.

    int finish();
    int finish(const uint64_t tsp);
    // This should be the last method called on the Span. It marks the end of
    // this Span (in microseconds since epoch) using the current wall-time, or
    // an explicit timestamp 'tsp'. Returns 0 upon success and a non-zero
    // value otherwise. Multiple calls to 'finish()' are redundant. The only
    // other method that may be called on the span, after 'finish()' is to
    // copy the 'context()'.

    template <typename T>
    int tag(const StringRef& key, const T& val);
    // Tag this span with the supplied key:val pair. Returns 0 upon success and
    // a non-zero value otherwise. It is undefined behavior to call this method
    // after 'finish()'.
    //
    // Note: Any type passed must support being externalized:
    // `std::ostream& operator<<(std::ostream& os, const T& val);

    template <typename T>
    int log(const StringRef& key, const T& val);
    // Log the supplied key:val pair on this span with the current wall-time.
    // Returns 0 upon success and a non-zero value otherwise. It is undefined
    // behavior to call this method after 'finish()'.
    //
    // Note: Any type passed must support being externalized:
    // `std::ostream& operator<<(std::ostream& os, const T& val);

    template <typename T>
    int log(const StringRef& key, const T& val, const uint64_t tsp);
    // Log the supplied key:val pair on this span with the supplied 'tsp'
    // timestamp. Returns 0 upon success and a non-zero value otherwise.
    // It is undefined behavior to call this method after 'finish()'.
    //
    // Note: Any type passed must support being externalized:
    // `std::ostream& operator<<(std::ostream& os, const T& val);

  protected:
    GenericSpan();
    GenericSpan(const GenericSpan&);
    // Protected to avoid direct construction
};

// -----------------
// class GenericSpan
// -----------------

template <typename SPAN, typename CONTEXT, typename ADAPTER>
inline GenericSpan<SPAN, CONTEXT, ADAPTER>::GenericSpan()
{
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
inline GenericSpan<SPAN, CONTEXT, ADAPTER>::GenericSpan(const GenericSpan&)
{
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
inline const typename GenericSpan<SPAN, CONTEXT, ADAPTER>::SpanContext*
GenericSpan<SPAN, CONTEXT, ADAPTER>::context() const
{
    return static_cast<const SPAN*>(this)->contextImp();
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
inline int
GenericSpan<SPAN, CONTEXT, ADAPTER>::setOperation(const StringRef& operation)
{
    return static_cast<SPAN*>(this)->setOperationImp(operation);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
inline int
GenericSpan<SPAN, CONTEXT, ADAPTER>::setBaggage(const StringRef& key,
                                                const StringRef& value)
{
    return static_cast<SPAN*>(this)->setBaggageImp(key, value);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
inline int
GenericSpan<SPAN, CONTEXT, ADAPTER>::getBaggage(const StringRef& key,
                                                std::string* const value) const
{
    return static_cast<const SPAN*>(this)->getBaggageImp(key, value);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
inline int
GenericSpan<SPAN, CONTEXT, ADAPTER>::getBaggage(
    const StringRef& key, std::vector<std::string>* const value) const
{
    return static_cast<const SPAN*>(this)->getBaggageImp(key, value);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
inline int
GenericSpan<SPAN, CONTEXT, ADAPTER>::finish()
{
    return static_cast<SPAN*>(this)->finishImp();
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
inline int
GenericSpan<SPAN, CONTEXT, ADAPTER>::finish(const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->finishImp(tsp);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename T>
inline int
GenericSpan<SPAN, CONTEXT, ADAPTER>::tag(const StringRef& key, const T& val)
{
    return static_cast<SPAN*>(this)->tagImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename T>
inline int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key, const T& val)
{
    return static_cast<SPAN*>(this)->logImp(key, val);
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
template <typename T>
inline int
GenericSpan<SPAN, CONTEXT, ADAPTER>::log(const StringRef& key,
                                         const T&       val,
                                         const uint64_t tsp)
{
    return static_cast<SPAN*>(this)->logImp(key, val, tsp);
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPAN_H
