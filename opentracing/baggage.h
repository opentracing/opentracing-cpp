#ifndef INCLUDED_OPENTRACING_BAGGAGE_H
#define INCLUDED_OPENTRACING_BAGGAGE_H

// =========
// baggage.h
// =========
// class Baggage    - Client wrapper for copies of key:value pairs
// class BaggageRef - Client wrapper for referencing key:value pairs
//
// class BaggageIteratorImp - Client interface to traverse span baggage
// class BaggageRangeImp    - A range of BaggageIterators

// -------------------
// OpenTracing Baggage
// -------------------
// Baggage items are key:value string pairs that apply to a given Span, its
// SpanContext, and all Spans which directly or transitively reference the local
// Span. That is, baggage items propagate in-band along with the trace itself.
//
// Baggage items enable powerful functionality given a full-stack OpenTracing
// integration (for example, arbitrary application data from a mobile
// application can make it, transparently, all the way into the depths of a
// storage system), and with it some powerful costs.
//
// Use this feature thoughtfully and with care. Every key and value is copied
// into every local and remote child of the associated Span, and that can add up
// to a lot of network and CPU overhead.
//
// See: https://github.com/opentracing/specification

#include <opentracing/stringref.h>

namespace opentracing {

// =============
// class Baggage
// =============
// This class copies to the key:value pairs stored in SpanContexts. Once the
// value is returned, clients are free to move it around as they see fit.
// Baggage is the default value returned by iterators when they are
// dereferenced.

class Baggage {
  public:
    Baggage();
    // Construct a Baggage with empty values.

    Baggage(const StringRef& key, const StringRef& value);
    // Construct a Baggage referencing 'key' and 'value'

    std::string& key();
    // Mutable reference to the underlying 'key'.

    std::string& value();
    // Mutable reference to the underlying 'value'.

    const std::string& key() const;
    // Return the non-modifiable 'key' associated with this baggage.

    const std::string& value() const;
    // Return the non-modifiable 'value' associated with this baggage.

    Baggage* operator->();
    const Baggage* operator->() const;
    // Syntactic sugar to support dereferencing BaggageIterator's with
    // the '->' operator: this method only returns 'this'.

  private:
    std::string m_key;
    std::string m_value;
};

// ================
// class BaggageRef
// ================
// This class wraps references to the key:value pairs stored in SpanContexts.
// The references themselves refer to data managed by SpanContext
// implementations, but make them available in a read-only fashion to avoid
// unnecessary copies.
//
// Since the implementations of SpanContexts is deferred, it is impossible to
// make strong guarantees on the lifetime of the references. At a minimum, the
// references should be valid until the iterator is destroyed, the SpanContext
// is destroyed, or until 'setBaggage' is called on the SpanContext.

class BaggageRef {
  public:
    BaggageRef();
    // Construct a BaggageRef with empty references.

    BaggageRef(const StringRef& key, const StringRef& value);
    // Construct a BaggageRef referencing 'key' and 'value'

    const StringRef& key() const;
    // Return the non-modifiable 'key' associated with this baggage.

    const StringRef& value() const;
    // Return the non-modifiable 'value' associated with this baggage.

    const BaggageRef* operator->() const;
    // Syntactic sugar to support dereferencing BaggageIterators with
    // the '->' operator: this method only returns 'this'.

  private:
    StringRef m_key;
    StringRef m_value;
};

// ========================
// class BaggageIteratorImp
// ========================
// BaggageIteratorImp is used to obscure the data structure(s) used to store
// and traverse Span Baggage. Implementors should store the values as
// std::string types, but the  underlying data structure decision is left to
// implementors.
//
// We do not want to expose the details of these data structure(s) to clients;
// If their code were to rely on any detail, it would make it difficult
// to change OpenTracing implementations. Instead, we define an Adapter class,
// to translate the implementation's iterators into baggage structures
// which clients can interact with.
//
// Adapter implementations are required to implement the following:
//
//  struct AdapterClass {
//      typedef ImplementationIterator iterator;
//      typedef ImplementationIterator const_iterator;
//
//      Baggage copy(const const_iterator& it) const;
//      BaggageRef ref(const const_iterator& it) const;
//  };
//
// When clients create a BaggageIterator via 'SpanContext::baggageBegin()',
// the BaggageIterator should be created with an iterator to the underlying
// baggage implementation. The BaggageIterator caches the implementation
// iterator, incrementing the implementation iterator under the hood as clients
// move forward through the underlying sequence.
//
// When clients dereference the baggage iterator, the Adapter is invoked to
// convert the implementation iterator into Baggage/BaggageRefs.

template <typename ADAPTER>
class BaggageIteratorImp {
  public:
    typedef typename ADAPTER::iterator       iterator;
    typedef typename ADAPTER::const_iterator const_iterator;

    BaggageIteratorImp(const iterator& iter);
    BaggageIteratorImp(const const_iterator& iter);
    // Construct a BaggageIteratorImp object which will be used to traverse the
    // sequence pointed to by the input iterator 'iter'.

    BaggageRef operator*() const;
    BaggageRef operator->() const;
    // Returns a BaggageRef to return references to the key:value pair pointed
    // to by this iterator. Undefined behavior if the iterator is equal to
    // 'end()'.

    Baggage copy() const;
    // Returns a copy of the key:value pairs pointed at by this iterator.
    // Undefined behavior if the iterator is equal to 'end()'.

    BaggageRef ref() const;
    // Returns a BaggageRef object, containing references to the underlying
    // storage of the key:value pairs. References are invalided if the
    // associated SpanContext is destroyed or modified.

    BaggageIteratorImp operator++(int);
    // Return a copy of this iterator, then post-increment this iterator to
    // point to the next key:value pair in the sequence.

    BaggageIteratorImp& operator++();
    // Increment this iterator, pointing to the next key:value pair in the
    // sequence, then return a reference to this iterator.

    bool operator==(const BaggageIteratorImp& other) const;
    // Returns true if this iterator points to the same key:value pair as
    // 'other', and false otherwise.

    bool operator!=(const BaggageIteratorImp& other) const;
    // Returns true if this iterator points at a different key:value pair
    // than 'other', and false otherwise.

  private:
    const_iterator d_iterator;  // Traverses implementation storage
    ADAPTER        d_handler;   // Converts d_iterator to Baggage wrappers
};

// =====================
// class BaggageRangeImp
// =====================
// If clients have access to C++11 features, BaggageRanges are a convenience
// class that allows an easy way to use range based for loops:
//
//   `for(const auto& b: context.baggageRange()){ ... }`
//
// They define a 'begin' and 'end' method that allows the compiler to use
// this syntax. The class is otherwise redundant.

template <typename ADAPTER>
class BaggageRangeImp {
  public:
    typedef BaggageIteratorImp<ADAPTER> BaggageIterator;

    BaggageRangeImp(const BaggageIterator& begin, const BaggageIterator& end);

    BaggageIterator begin() const;
    BaggageIterator end() const;

  private:
    const BaggageIterator m_begin;
    const BaggageIterator m_end;
};

// -------------
// class Baggage
// -------------

inline Baggage::Baggage() : m_key(), m_value()
{
}

inline Baggage::Baggage(const StringRef& key, const StringRef& value)
: m_key(key.data(), key.length()), m_value(value.data(), value.length())
{
}

inline std::string&
Baggage::key()
{
    return m_key;
}

inline std::string&
Baggage::value()
{
    return m_value;
}

inline const std::string&
Baggage::key() const
{
    return m_key;
}

inline const std::string&
Baggage::value() const
{
    return m_value;
}

inline Baggage* Baggage::operator->()
{
    return this;
}

inline const Baggage* Baggage::operator->() const
{
    return this;
}

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

// ------------------------
// class BaggageIteratorImp
// ------------------------

template <typename ADAPTER>
inline BaggageIteratorImp<ADAPTER>::BaggageIteratorImp(const iterator& iter)
: d_iterator(iter), d_handler()
{
}

template <typename ADAPTER>
inline BaggageIteratorImp<ADAPTER>::BaggageIteratorImp(
    const const_iterator& iter)
: d_iterator(iter), d_handler()
{
}

template <typename ADAPTER>
inline BaggageRef BaggageIteratorImp<ADAPTER>::operator*() const
{
    return d_handler.ref(d_iterator);
}

template <typename ADAPTER>
inline BaggageRef BaggageIteratorImp<ADAPTER>::operator->() const
{
    return d_handler.ref(d_iterator);
}

template <typename ADAPTER>
inline Baggage
BaggageIteratorImp<ADAPTER>::copy() const
{
    return d_handler.copy(d_iterator);
}

template <typename ADAPTER>
inline BaggageRef
BaggageIteratorImp<ADAPTER>::ref() const
{
    return d_handler.ref(d_iterator);
}

template <typename ADAPTER>
inline BaggageIteratorImp<ADAPTER> BaggageIteratorImp<ADAPTER>::operator++(int)
{
    BaggageIteratorImp tmp(*this);
    ++d_iterator;
    return tmp;
}

template <typename ADAPTER>
inline BaggageIteratorImp<ADAPTER>& BaggageIteratorImp<ADAPTER>::operator++()
{
    ++d_iterator;
    return *this;
}

template <typename ADAPTER>
inline bool
BaggageIteratorImp<ADAPTER>::operator==(
    const BaggageIteratorImp<ADAPTER>& other) const
{
    return d_iterator == other.d_iterator;
}

template <typename ADAPTER>
inline bool
BaggageIteratorImp<ADAPTER>::operator!=(
    const BaggageIteratorImp<ADAPTER>& other) const
{
    return d_iterator != other.d_iterator;
}

// ---------------------
// class BaggageRangeImp
// ---------------------

template <typename ADAPTER>
inline BaggageRangeImp<ADAPTER>::BaggageRangeImp(const BaggageIterator& begin,
                                                 const BaggageIterator& end)
: m_begin(begin), m_end(end)
{
}

template <typename ADAPTER>
inline typename BaggageRangeImp<ADAPTER>::BaggageIterator
BaggageRangeImp<ADAPTER>::begin() const
{
    return m_begin;
}

template <typename ADAPTER>
inline typename BaggageRangeImp<ADAPTER>::BaggageIterator
BaggageRangeImp<ADAPTER>::end() const
{
    return m_end;
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_BAGGAGE_H
