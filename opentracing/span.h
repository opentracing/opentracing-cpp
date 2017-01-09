#ifndef INCLUDED_OPENTRACING_SPAN_H
#define INCLUDED_OPENTRACING_SPAN_H

// ======
// span.h
// ======
// class GenericSpan - CRTP interface for Spans

#include <opentracing/spancontext.h>
#include <opentracing/spanoptions.h>
#include <opentracing/stringref.h>

#include <opentracing/config.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

namespace opentracing {

// =================
// class GenericSpan
// =================
// GenericSpan is a static, polymorphic interface for interacting with
// Spans. It uses the Curiously Repeating Template Patttern (CRTP) to
// avoid v-table hits we would encounter with traditional polymorphism.
//
// See this CRTP article for more details on the design pattern:
// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
//
// The GenericSpan has three template parameters:
//   * SPAN    - A Span implementation derived class (CRTP)
//   * CONTEXT - A SpanContext implementation derived class (CRTP)
//   * ADAPTER - A Baggage iterator adapter
//
// The CONTEXT and ADAPTER are used to alias the related GenericSpanContext
// class as the SpanContext. This is a convenience typedef to ensure what is
// exposed to clients is the same as the type used by the interface.
//
// SPAN implementations must support the following:
//
// class SpanImpl: GenericSpan<SpanImpl, ContextImpl, Adapter>
// {
//   public:
//      ContextImpl& contextImp();
//      const ContextImpl& contextImp() const;
//
//      int addReferenceImp(const ContextImpl&, const SpanRelationship::Value);
//      int setOperationImp(const StringRef& operation);
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
//      `std::ostream& operator<<(std::ostream&, const T& val);`

template <typename SPAN, typename CONTEXT, typename ADAPTER>
class GenericSpan {
  public:
    typedef GenericSpanContext<CONTEXT, ADAPTER> SpanContext;

    SpanContext&       context();
    const SpanContext& context() const;
    // Return the SpanContext associated with this span. Baggage can be
    // retrieved/modified through the SpanContext. The context can be injected
    // into carriers, or added as a reference to other causally related Spans.
    // It is undefined behavior to modify the context after 'finish()' has been
    // called on this Span.

    int setOperation(const StringRef& operation);
    // Modify the Span's operation. Return 0 upon success and a non-zero
    // value otherwise. It is undefined behavior to call this method after
    // 'finish()'.

    int addReference(const SpanContext&            context,
                     const SpanRelationship::Value relationship);
    // Add a reference to another SpanContext, after the Span was created.
    // Returns 0 upon success and a non-zero value otherwise.
    // It is undefined behavior to call this method after 'finish()'.

    int finish();
    int finish(const uint64_t tsp);
    // This should be the last method called on the Span. It marks the end of
    // this Span (in microseconds since epoch) using the current wall-time, or
    // an explicit timestamp 'tsp'. Retrusn 0 upon success and a non-zero
    // value otherwise. It is undefined behavior to call any method on this span
    // after 'finish()' is called, except for accessing 'context()' in a
    // read-only fashion.

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
inline typename GenericSpan<SPAN, CONTEXT, ADAPTER>::SpanContext&
GenericSpan<SPAN, CONTEXT, ADAPTER>::context()
{
    return static_cast<SPAN*>(this)->contextImp();
}

template <typename SPAN, typename CONTEXT, typename ADAPTER>
inline const typename GenericSpan<SPAN, CONTEXT, ADAPTER>::SpanContext&
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
GenericSpan<SPAN, CONTEXT, ADAPTER>::addReference(
    const SpanContext& context, const SpanRelationship::Value relationship)
{
    const CONTEXT& contextImp = static_cast<const CONTEXT&>(context);
    return static_cast<SPAN*>(this)->addReferenceImp(contextImp, relationship);
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
