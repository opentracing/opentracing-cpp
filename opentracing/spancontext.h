#ifndef INCLUDED_OPENTRACING_SPANCONTEXT_H
#define INCLUDED_OPENTRACING_SPANCONTEXT_H

// =============
// spancontext.h
// =============
//
// class GenericSpanContext - CRTP interface for SpanContexts
//
// -----------
// SpanContext
// -----------
// Every Span has a SpanContext. This context is used to:
//   * Inject Spans into Carriers
//   * Represent a Span in-band, across process boundaries
//   * Extract Spans from Carriers
//   * Create new Spans, related to the SpanContext
//
// Since the Span has to represent the entire Span in-band, and baggage must
// also be propagated across process boundaries, the SpanContext is also
// responsible for baggage.
//
// In most OpenTracing implementations, the SpanContext is made immutable in
// order to avoid complicated lifetime issues. The C++ interface relaxes those
// requirements, exposing the 'GenericSpanContext::setBaggage()' method directly
// on the context.
//
// See the specification for more details:
// https://github.com/opentracing/specification/blob/master/specification.md#Spancontext

#include <opentracing/stringref.h>
#include <opentracing/baggage.h>

namespace opentracing {

// ========================
// class GenericSpanContext
// ========================
// GenericSpanContext is a static, polymorphic interface for interacting with
// SpanContexts. It uses the Curiously Repeating Template Pattern (CRTP) to
// avoid unnecessary v-table hits we would encounter with traditional
// polymorphism.
//
// See this CRTP article for more details on the design pattern:
// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
//
// -------
// Clients
// -------
// After a Span is created, the 'Span::context()' method is used to access
// a context that supports the 'GenericSpanContext' interface.
//
// Clients should never need to reference the GenericSpanContext type
// explicitly, but instead, clients should use the Tracer::Span::SpanContext
// typedef. By using the typedef, clients will avoid code changes necessary if a
// different Tracer implementation is installed later.
//
// Once clients have access to a SpanContext, they can:
//   * 'setBaggage()' key:value pairs
//   * 'getBaggage()' for a given key
//   * Iterate over all Baggage via 'begin()'/'end()'
//
// For more details on:
//   * accessing a SpanContext, see 'span.h'
//   * iterating over Baggage, see 'baggage.h'
//   * inject/extract, see 'tracer.h
//
// ------------
// Implementors
// ------------
// The 'GenericSpanContext' has two template parameters:
//   * CONTEXT - A SpanContext implementation derived class (CRTP)
//   * ADAPTER - A BaggageIterator adapter
//
// CONTEXT implementations are required to implement the following:
//
//   class ContextImpl : GenericSpanContext<ContextImpl, Adapter>
//   {
//     public:
//       const_iterator beginImp() const;
//       const_iterator endImp() const;
//       int setBaggageImp(const StringRef& const, const StringRef& baggage);
//       int getBaggageImp(StringRef *const, const StringRef& key) const;
//   };
//
// Implementations may choose how they implement Baggage, but the
// beginImp/endImp iterators should interact with the installed
// BaggageIterator/Adapter correctly.
//
// Since the SpanContext implementation type is decided at compile time, Span
// and Tracer implementations may safely static_cast GenericSpanContext
// references down to the installed SpanContext implementation. Doing so allows
// access to methods/members that would be otherwise inaccessible through the
// base class interface. This is a useful tool when implementing inject/extract
// in Tracers.

template <typename CONTEXT, typename ADAPTER>
class GenericSpanContext {
  public:
    typedef CONTEXT                  SpanContext;
    typedef ADAPTER                  BaggageAdapter;
    typedef BaggageIterator<ADAPTER> const_iterator;

    const_iterator begin() const;
    // Return a BaggageIterator to the beginning of the Baggage maintained
    // by this SpanContext.

    const_iterator end() const;
    // Return a BaggageIterator to mark the end of the baggage maintained
    // by this SpanContext.

    int setBaggage(const StringRef& key, const StringRef& baggage);
    // Associate the supplied 'key' with the 'baggage'. Returns 0 upon
    // success and a non-zero value otherwise.

    int getBaggage(StringRef* const baggage, const StringRef& key) const;
    // Load the 'baggage' associated with 'key'. Returns 0 if the
    // baggage is loaded succesfully, and a non-zero value otherwise.

  protected:
    GenericSpanContext();
    GenericSpanContext(const GenericSpanContext&);
    // Protected to avoid direct construction
};

// ------------------------
// class GenericSpanContext
// ------------------------

template <typename CONTEXT, typename ADAPTER>
GenericSpanContext<CONTEXT, ADAPTER>::GenericSpanContext()
{
}

template <typename CONTEXT, typename ADAPTER>
GenericSpanContext<CONTEXT, ADAPTER>::GenericSpanContext(
    const GenericSpanContext&)
{
}

template <typename CONTEXT, typename ADAPTER>
typename GenericSpanContext<CONTEXT, ADAPTER>::const_iterator
GenericSpanContext<CONTEXT, ADAPTER>::begin() const
{
    return const_iterator(static_cast<const CONTEXT*>(this)->beginImp());
}

template <typename CONTEXT, typename ADAPTER>
typename GenericSpanContext<CONTEXT, ADAPTER>::const_iterator
GenericSpanContext<CONTEXT, ADAPTER>::end() const
{
    return const_iterator(static_cast<const CONTEXT*>(this)->endImp());
}

template <typename CONTEXT, typename ADAPTER>
int
GenericSpanContext<CONTEXT, ADAPTER>::setBaggage(const StringRef& key,
                                                 const StringRef& baggage)
{
    return static_cast<CONTEXT*>(this)->setBaggageImp(key, baggage);
}

template <typename CONTEXT, typename ADAPTER>
int
GenericSpanContext<CONTEXT, ADAPTER>::getBaggage(StringRef* const baggage,
                                                 const StringRef& key) const
{
    return static_cast<const CONTEXT*>(this)->getBaggageImp(baggage, key);
}
}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPANCONTEXT_H
