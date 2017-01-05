#ifndef INCLUDED_OPENTRACING_SPANCONTEXT_H
#define INCLUDED_OPENTRACING_SPANCONTEXT_H

#include <opentracing/baggage.h>
#include <opentracing/stringref.h>

namespace opentracing {

template <typename IMPL, typename ADAPTER>
class GenericSpanContext {
  public:
    typedef IMPL                     SpanContext;
    typedef ADAPTER                  BaggageAdapter;
    typedef BaggageIterator<ADAPTER> const_iterator;

    virtual ~GenericSpanContext();

    const_iterator begin() const;
    const_iterator end() const;
    // Return iterators to baggage managed by this SpanContext.

    int setBaggage(const StringRef& key, const StringRef& baggage);
    // Associate the supplied 'key' with the 'baggage'.

    int getBaggage(StringRef* const baggage, const StringRef& key) const;
    // Load the 'baggage' associated with 'key'.

  protected:
    GenericSpanContext();
    GenericSpanContext(const GenericSpanContext&);
    // Protected to avoid direct construction
};

template <typename IMPL, typename ADAPTER>
GenericSpanContext<IMPL, ADAPTER>::~GenericSpanContext()
{
}

template <typename IMPL, typename ADAPTER>
GenericSpanContext<IMPL, ADAPTER>::GenericSpanContext()
{
}

template <typename IMPL, typename ADAPTER>
GenericSpanContext<IMPL, ADAPTER>::GenericSpanContext(const GenericSpanContext&)
{
}

template <typename IMPL, typename ADAPTER>
typename GenericSpanContext<IMPL, ADAPTER>::const_iterator
GenericSpanContext<IMPL, ADAPTER>::begin() const
{
    return const_iterator(static_cast<const IMPL*>(this)->beginImp());
}

template <typename IMPL, typename ADAPTER>
typename GenericSpanContext<IMPL, ADAPTER>::const_iterator
GenericSpanContext<IMPL, ADAPTER>::end() const
{
    return const_iterator(static_cast<const IMPL*>(this)->endImp());
}

template <typename IMPL, typename ADAPTER>
int
GenericSpanContext<IMPL, ADAPTER>::setBaggage(const StringRef& key,
                                              const StringRef& baggage)
{
    return static_cast<IMPL*>(this)->setBaggageImp(key, baggage);
}

template <typename IMPL, typename ADAPTER>
int
GenericSpanContext<IMPL, ADAPTER>::getBaggage(StringRef* const baggage,
                                              const StringRef& key) const
{
    return static_cast<const IMPL*>(this)->getBaggageImp(baggage, key);
}
}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPANCONTEXT_H
