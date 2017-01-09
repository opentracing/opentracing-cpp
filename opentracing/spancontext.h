#ifndef INCLUDED_OPENTRACING_SPANCONTEXT_H
#define INCLUDED_OPENTRACING_SPANCONTEXT_H

// =============
// spancontext.h
// =============
// class GenericSpanContext - CRTP interface for SpanContexts

#include <opentracing/baggage.h>
#include <opentracing/stringref.h>
#include <string>
#include <vector>

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
// The 'GenericSpanContext' has two template parameters:
//   * CONTEXT - A SpanContext implementation derived class (CRTP)
//   * ADAPTER - A BaggageIteratorImp adapter
//
// CONTEXT implementations are required to implement the following:
//
//   class ContextImpl : GenericSpanContext<ContextImpl, Adapter>
//   {
//     public:
//       typename Adapter::const_iterator baggageBeginImp() const;
//       typename Adapter::const_iterator baggageEndImp() const;
//
//       int setBaggageImp(const StringRef&, const StringRef&);
//
//       int getBaggageImp(std::string* const, const StringRef&) const;
//       int getBaggageImp(BaggageValues* const, const StringRef&) const;
//   };
//
// Implementations may choose how they implement storage of Baggage, but the
// baggageBeginImp/baggageEndImp iterators should interact with the installed
// BaggageIterator/Adapter correctly.

template <typename CONTEXT, typename ADAPTER>
class GenericSpanContext {
  public:
    typedef CONTEXT                     SpanContext;
    typedef ADAPTER                     BaggageAdapter;
    typedef BaggageIteratorImp<ADAPTER> BaggageIterator;
    typedef BaggageRangeImp<ADAPTER>    BaggageRange;

    typedef std::string               BaggageValue;
    typedef std::vector<BaggageValue> BaggageValues;

    BaggageIterator baggageBegin() const;
    // Return a BaggageIterator to the beginning of the Baggage maintained
    // by this SpanContext.

    BaggageIterator baggageEnd() const;
    // Return a BaggageIterator to mark the end of the Baggage maintained
    // by this SpanContext.

    BaggageRange baggageRange() const;
    // Return a structure containing the range of iterators:
    // [baggageBegin, baggageEnd). The object supports range-based for loops.

    int setBaggage(const StringRef &key, const StringRef &baggage);
    // Set or append the single 'baggage' value for the given 'key'. Return 0
    // upon success and a non-zero value otherwise.

    int getBaggage(BaggageValue *const baggage, const StringRef &key) const;
    // Load a single 'baggage' value associated with 'key'. Returns 0 if there
    // is only one value associated with 'key' and that value was loaded
    // succesfully. Return a non-zero value otherwise.

    int getBaggage(BaggageValues *const baggage, const StringRef &key) const;
    // Load the 'baggage' associated with 'key'. Returns 0 if the
    // baggage is loaded succesfully, and a non-zero value otherwise.

  protected:
    GenericSpanContext();
    GenericSpanContext(const GenericSpanContext &);
    // Protected to avoid direct construction
};

// ------------------------
// class GenericSpanContext
// ------------------------

template <typename CONTEXT, typename ADAPTER>
inline GenericSpanContext<CONTEXT, ADAPTER>::GenericSpanContext()
{
}

template <typename CONTEXT, typename ADAPTER>
inline GenericSpanContext<CONTEXT, ADAPTER>::GenericSpanContext(
    const GenericSpanContext &)
{
}

template <typename CONTEXT, typename ADAPTER>
inline typename GenericSpanContext<CONTEXT, ADAPTER>::BaggageIterator
GenericSpanContext<CONTEXT, ADAPTER>::baggageBegin() const
{
    return BaggageIterator(
        static_cast<const CONTEXT *>(this)->baggageBeginImp());
}

template <typename CONTEXT, typename ADAPTER>
inline typename GenericSpanContext<CONTEXT, ADAPTER>::BaggageIterator
GenericSpanContext<CONTEXT, ADAPTER>::baggageEnd() const
{
    return BaggageIterator(static_cast<const CONTEXT *>(this)->baggageEndImp());
}

template <typename CONTEXT, typename ADAPTER>
inline typename GenericSpanContext<CONTEXT, ADAPTER>::BaggageRange
GenericSpanContext<CONTEXT, ADAPTER>::baggageRange() const
{
    return BaggageRange(baggageBegin(), baggageEnd());
}

template <typename CONTEXT, typename ADAPTER>
inline int
GenericSpanContext<CONTEXT, ADAPTER>::setBaggage(const StringRef &key,
                                                 const StringRef &baggage)
{
    return static_cast<CONTEXT *>(this)->setBaggageImp(key, baggage);
}

template <typename CONTEXT, typename ADAPTER>
inline int
GenericSpanContext<CONTEXT, ADAPTER>::getBaggage(BaggageValue *const baggage,
                                                 const StringRef &key) const
{
    return static_cast<const CONTEXT *>(this)->getBaggageImp(baggage, key);
}

template <typename CONTEXT, typename ADAPTER>
inline int
GenericSpanContext<CONTEXT, ADAPTER>::getBaggage(BaggageValues *const baggage,
                                                 const StringRef &key) const
{
    return static_cast<const CONTEXT *>(this)->getBaggageImp(baggage, key);
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPANCONTEXT_H
