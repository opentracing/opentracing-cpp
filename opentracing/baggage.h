#ifndef INCLUDED_OPENTRACING_BAGGAGE_H
#define INCLUDED_OPENTRACING_BAGGAGE_H

// =========
// baggage.h
// =========
// class BaggageRef      - Client wrapper for referencing key:value pairs
// class BaggageIterator - Client interface to traverse span baggage
//
// -------------------
// OpenTracing Baggage
// -------------------
// Baggage items are key:value string pairs that apply to a given Span, its
// SpanContext, and all Spans which directly or transitively reference the local
// Span. That is, baggage items propagate in-band along with the trace itself.
//
// Baggage items enable powerful functionality given a full-stack OpenTracing
// integration (for example, arbitrary application data from a mobile app can
// make it, transparently, all the way into the depths of a storage system), and
// with it some powerful costs: use this feature with care.
//
// Use this feature thoughtfully and with care. Every key and value is copied
// into every local and remote child of the associated Span, and that can add up
// to a lot of network and cpu overhead.
//
// See: https://github.com/opentracing/specification

#include <opentracing/stringref.h>

namespace opentracing {

// ================
// class BaggageRef
// ================
//
// This class wraps references to the key:value pairs that make up 'baggage'.
// The references themselves refer to data managed by SpanContext
// implementations.
//
// Since the implementations of SpanContexts is deferred, it is impossible to
// make strong gurantees on the liftetime of the references. At a minimum, the
// references should be valid until the iterator is destroyed, or until
// 'setBaggage' is called again on a SpanContext.

class BaggageRef {
  public:
    BaggageRef();
    // Construct a 'BaggageRef' with empty references.

    BaggageRef(const StringRef& key, const StringRef& value);
    // Construct a 'BaggageRef' referencing 'key' and 'value'

    const StringRef& key() const;
    // Return the non-modifiable 'key' associated with this baggage.

    const StringRef& value() const;
    // Return the non-modifiable 'value' associated with this baggage.

    const BaggageRef* operator->() const;
    // Syntatic sugar to support dereferencing BaggageIterator's with
    // the '->' operator: this method only returns 'this'.

  private:
    StringRef m_key;
    StringRef m_value;
};

// =====================
// class BaggageIterator
// =====================
// -------
// Clients
// -------
// For flexibility between implementations, 'BaggageIterator' classes should
// never be created directly. Instead, a typedef is provided by 'SpanContext's
// which allow callers to be agnostic of the underlying implementation.
// By using 'SpanContext::const_iterator', the call sites will be unaffected
// if implementations change in the future.
//
// The iterators themselves satisfy the requirements of C++ "Input Iterators".
// They can read from the pointed-to baggage element, returning 'BaggageRef' for
// the key:value pairs.
//
// Those references only guarantee validity for a single pass: once an iterator
// has been incremented, all copies of its previous value may be invalidated.
//
// See http://en.cppreference.com/w/cpp/concept/InputIterator for more details
// on semantics input iterators.
//
// ------------
// Implementors
// ------------
// 'BaggageIterator' is used to obscure the data structure(s) used to store
// and traverse span baggage. Implementors are free to store the key:value
// baggage pairs however they see fit, so long as they provide an
// "Input Iterator" to traverse the structure.
//
// We do not want to expose the details of these data structure(s) to clients;
// If their code were to rely on any detail, it would make it difficult
// to change opentracing implementations. Instead, we define an 'Adapter'
// class, to translate the implementation's iterator into baggage references.
//
// The 'Adapter' class must satisfy the following:
//   1) Provide an 'iterator' typedef, potentially used during construction
//   2) Provide a 'const_iterator' typedef, potentially used during
//      construction.
//   3) Overload operator() to convert a const_iterator into a BaggageRef
//
// To achieve this, implementors define a class like so:
//
//  struct AdapterClass {
//      typedef ImplementationIterator iterator;
//      typedef ImplementationIterator const_iterator;
//      BaggageRef operator()(const const_iterator& it) const;
//  };
//
// Now, when clients create a 'BaggageIterator' through 'SpanContext::begin()',
// the BaggageIterator should be created with an iterator to the underlying
// baggage implementation. It caches the iterator, incrementing it as
// clients move forward through the sequence.
//
// When clients dreference the baggage iterator, the 'Adapter' is invoked,
// passing the implementation's iterator to the adapter. It is expected that the
// 'Adapter' can convert the iterator into a BaggageRef that client code can
// use in a read-only fashion.

template <typename ADAPTER>
class BaggageIterator {
  public:
    typedef typename ADAPTER::iterator       iterator;
    typedef typename ADAPTER::const_iterator const_iterator;

    BaggageIterator(const iterator& iter);
    BaggageIterator(const const_iterator& iter);
    // Construct a 'BaggageIterator' object which will be used to traverse the
    // sequence pointed to by the input iterator 'iter'.

    BaggageRef operator*() const;
    BaggageRef operator->() const;
    // Returns a 'BaggageRef' to return the key:value pair pointed to by this
    // iterator.

    BaggageIterator operator++(int);
    // Return a copy of this iterator, then post-increment this iterator to
    // point to the next key:value pair in the sequence.

    BaggageIterator& operator++();
    // Increment this iterator, pointing to the next key:value pair in the
    // sequence.

    bool operator==(const BaggageIterator& other) const;
    // Returns true if this iterator points to the same key:value pair as
    // 'other', and false otherwise.

    bool operator!=(const BaggageIterator& other) const;
    // Returns true if this iterator points at a different key:value pair
    // than 'other', and false otherwise.

  private:
    const_iterator d_iterator;  // Traverses implementation storage
    ADAPTER        d_handler;   // Converts 'd_iterator' to BaggageRef
};

// ----------------
// class BaggageRef
// ----------------

inline BaggageRef::BaggageRef() : m_key(), m_value()
{
}

inline BaggageRef::BaggageRef(const StringRef& key, const StringRef& value)
: m_key(key), m_value(value)
{
}

inline const StringRef&
BaggageRef::key() const
{
    return m_key;
}

inline const StringRef&
BaggageRef::value() const
{
    return m_value;
}

inline const BaggageRef* BaggageRef::operator->() const
{
    return this;
}

// ---------------------
// class BaggageIterator
// ---------------------

template <typename ADAPTER>
BaggageIterator<ADAPTER>::BaggageIterator(const iterator& iter)
: d_iterator(iter), d_handler()
{
}

template <typename ADAPTER>
BaggageIterator<ADAPTER>::BaggageIterator(const const_iterator& iter)
: d_iterator(iter), d_handler()
{
}

template <typename ADAPTER>
BaggageRef BaggageIterator<ADAPTER>::operator*() const
{
    return d_handler(d_iterator);
}

template <typename ADAPTER>
BaggageRef BaggageIterator<ADAPTER>::operator->() const
{
    return d_handler(d_iterator);
}

template <typename ADAPTER>
BaggageIterator<ADAPTER> BaggageIterator<ADAPTER>::operator++(int)
{
    BaggageIterator tmp(*this);
    ++d_iterator;
    return tmp;
}

template <typename ADAPTER>
BaggageIterator<ADAPTER>& BaggageIterator<ADAPTER>::operator++()
{
    ++d_iterator;
    return *this;
}

template <typename ADAPTER>
bool
BaggageIterator<ADAPTER>::operator==(
    const BaggageIterator<ADAPTER>& other) const
{
    return d_iterator == other.d_iterator;
}

template <typename ADAPTER>
bool
BaggageIterator<ADAPTER>::operator!=(
    const BaggageIterator<ADAPTER>& other) const
{
    return d_iterator != other.d_iterator;
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_BAGGAGE_H
