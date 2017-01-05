#ifndef INCLUDED_OPENTRACING_SPANCONTEXT_H
#define INCLUDED_OPENTRACING_SPANCONTEXT_H

#include <opentracing/baggage.h>
#include <opentracing/stringref.h>

namespace opentracing {

template <typename IMPL>
class GenericSpanContext {
  public:
    typedef IMPL                             SpanContext;
    typedef SpanContext::BaggageIteratorImpl IteratorImpl;
    typedef SpanContext::BaggageHandler      BaggageHandler;

    typedef BaggageIterator<BaggageHandler, IteratorImpl> const_iterator;

    typedef BaggageIterator const_iterator;

    const_iterator begin() const;
    const_iterator end() const;
    // Return iterators to baggage managed by this span_context.

    int setBaggage(const StringRef& key, const StringRef& baggage);
    // Associate the supplied 'key' with the 'baggage'.

    int getBaggage(StringRef* const baggage, const StringRef& key);
    // Load the 'baggage' associated with 'key'.

  private:
    GenericSpanContext();
    GenericSpanContext(const GenericSpanContext&);
    // Protected to avoid direct construction
};

template <typename IMPL>
GenericSpanContext::GenericSpanContext()
{
}

template <typename IMPL>
GenericSpanContext::GenericSpanContext(const GenericSpanContext&)
{
}

template <typename IMPL>
GenericSpanContext::const_iterator
GenericSpanContext::begin() const
{
    return const_iterator(static_cast<span_context*>(this)->begin());
}

template <typename IMPL>
GenericSpanContext::const_iterator
GenericSpanContext::end() const
{
    return const_iterator(static_cast<span_context*>(this)->end());
}

template <typename IMPL>
int
GenericSpanContext::setBaggage(const StringRef& key, const StringRef& baggage)
{
    return static_cast<span_context*>(this)->setBaggage(key, baggage);
}

template <typename IMPL>
int
GenericSpanContext::get_baggage(StringRef* const baggage,
                                const StringRef& key) const
{
    return static_cast<span_context*>(this)->get_baggage(baggage, key);
}
}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPANCONTEXT_H
