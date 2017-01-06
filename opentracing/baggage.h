#ifndef INCLUDED_OPENTRACING_BAGGAGE_H
#define INCLUDED_OPENTRACING_BAGGAGE_H

// =========
// baggage.h
// =========
// class BaggageImp      - Client wrapper for copies of key:value pairs
// class Baggage         - Typedef for Baggage working with char
// class BaggageWide     - Typedef for Baggage working with wchar_t
//
// class BaggageRefImp   - Client wrapper for referencing key:value pairs
// class BaggageRef      - Typedef for BaggageRef working with char
// class BaggageRefWide  - Typedef for BaggageRef working with wchar_t
//
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
// integration (for example, arbitrary application data from a mobile
// application can make it, transparently, all the way into the depths of a
// storage system), and with it some powerful costs: use this feature with care.
//
// Use this feature thoughtfully and with care. Every key and value is copied
// into every local and remote child of the associated Span, and that can add up
// to a lot of network and cpu overhead.
//
// See: https://github.com/opentracing/specification

#include <opentracing/stringref.h>

namespace opentracing {

// ================
// class BaggageImp
// ================
// This class copies to the key:value pairs stored in SpanContexts. Once the
// value is returned, clients are free to move it around as they see fit.
// Baggage is the default value returned by iterators when they are
// dereferenced.

template <typename CHAR>
class BaggageImp {
  public:
    BaggageImp();
    // Construct a Baggage with empty values.

    BaggageImp(const StringRefImp<CHAR>& key, const StringRefImp<CHAR>& value);
    // Construct a Baggage referencing 'key' and 'value'

    std::basic_string<CHAR>& key();
    // Mutable reference to the underlying 'key'.

    std::basic_string<CHAR>& value();
    // Mutable reference to the underlying 'value'.

    const std::basic_string<CHAR>& key() const;
    // Return the non-modifiable 'key' associated with this baggage.

    const std::basic_string<CHAR>& value() const;
    // Return the non-modifiable 'value' associated with this baggage.

    BaggageImp* operator->();
    const BaggageImp* operator->() const;
    // Syntactic sugar to support dereferencing BaggageIterator's with
    // the '->' operator: this method only returns 'this'.

  private:
    std::basic_string<CHAR> m_key;
    std::basic_string<CHAR> m_value;
};

typedef BaggageImp<char>    Baggage;
typedef BaggageImp<wchar_t> BaggageWide;

// ===================
// class BaggageRefImp
// ===================
// This class wraps references to the key:value pairs stored in SpanContexts.
// The references themselves refer to data managed by SpanContext
// implementations, but make them available in a read-only fashion to avoid
// unnecessary copies.
//
// Since the implementations of SpanContexts is deferred, it is impossible to
// make strong guarantees on the lifetime of the references. At a minimum, the
// references should be valid until the iterator is destroyed, the SpanContext
// is destoryed, or until 'setBaggage' is called on the SpanContext.

template <typename CHAR>
class BaggageRefImp {
  public:
    BaggageRefImp();
    // Construct a BaggageRef with empty references.

    BaggageRefImp(const StringRefImp<CHAR>& key,
                  const StringRefImp<CHAR>& value);
    // Construct a BaggageRef referencing 'key' and 'value'

    const StringRefImp<CHAR>& key() const;
    // Return the non-modifiable 'key' associated with this baggage.

    const StringRefImp<CHAR>& value() const;
    // Return the non-modifiable 'value' associated with this baggage.

    const BaggageRefImp* operator->() const;
    // Syntactic sugar to support dereferencing BaggageIterators with
    // the '->' operator: this method only returns 'this'.

  private:
    StringRefImp<CHAR> m_key;
    StringRefImp<CHAR> m_value;
};

typedef BaggageRefImp<char> BaggageRef;
typedef BaggageRefImp<wchar_t> BaggageRefWide;

// =====================
// class BaggageIterator
// =====================
// -------
// Clients
// -------
// For flexibility between implementations, BaggageIterator classes should
// never be created directly. Instead, a typedef is provided by SpanContexts
// which allow callers to be agnostic of the underlying implementation.
// By using 'SpanContext::const_iterator', the call sites will be unaffected
// if implementations change in the future.
//
// The iterators themselves satisfy the requirements of C++ "Input Iterators".
// They can read from the pointed-to baggage element, returning Baggage for
// the key:value pairs.
//
// BaggageRef objects may be returned instead of copying the Baggage with
// calls to 'ref()'. Those references only guarantee validity for a single pass:
// once an iterator has been incremented, all copies of its previous value may
// be invalidated.
//
// See http://en.cppreference.com/w/cpp/concept/InputIterator for more details
// on semantics input iterators.
//
// ------------
// Implementors
// ------------
// BaggageIterator is used to obscure the data structure(s) used to store
// and traverse span baggage. Implementors should store the values as
// std::string types. The underlying data structure decision is left to
// implementors.
//
// We do not want to expose the details of these data structure(s) to clients;
// If their code were to rely on any detail, it would make it difficult
// to change OpenTracing implementations. Instead, we define an 'Adapter'
// class, to translate the implementation's iterator into baggage references.
//
// The 'Adapter' class must satisfy the following:
//   * Provide an iterator typedef, potentially used during construction
//   * Provide a const_iterator typedef, potentially used during
//      construction.
//   * Provide narrow() to convert a const_iterator into Baggage
//   * Provide wide() to convert a const_iterator into BaggageWide
//   * Provide ref() to convert a const_iterator into a BaggageRef
//
// To achieve this, implementors define a class like so:
//
//  struct AdapterClass {
//      typedef ImplementationIterator iterator;
//      typedef ImplementationIterator const_iterator;
//
//      Baggage narrow()(const const_iterator& it) const;
//      Baggage wide()(const const_iterator& it) const;
//      BaggageRef ref()(const const_iterator& it) const;
//  };
//
// Now, when clients create a BaggageIterator through 'SpanContext::begin()',
// the BaggageIterator should be created with an iterator to the underlying
// baggage implementation. It caches the iterator, incrementing it as
// clients move forward through the sequence.
//
// When clients dereference the baggage iterator, the 'Adapter' is invoked,
// passing the implementation's iterator to the adapter. It is expected that the
// 'Adapter' can convert the iterator into a Baggage, BaggageWide, or BaggageRef
// object that client code can use in a read-only fashion.

template <typename ADAPTER>
class BaggageIterator {
  public:
    typedef typename ADAPTER::iterator       iterator;
    typedef typename ADAPTER::const_iterator const_iterator;

    BaggageIterator(const iterator& iter);
    BaggageIterator(const const_iterator& iter);
    // Construct a BaggageIterator object which will be used to traverse the
    // sequence pointed to by the input iterator 'iter'.

    BaggageRef operator*() const;
    BaggageRef operator->() const;
    // Returns a BaggageRef to return references to the key:value pair pointed
    // to by this iterator. Undefined behavior if the iterator is equal to
    // 'end()'.

    Baggage narrow() const;
    // Returns a copy of the key:value pairs pointed at by this iterator.
    // Undefined behavior if the iterator is equal to 'end()'.

    BaggageWide wide() const;
    // Returns a wide copies of the key:value pairs pointed at by this iterator.
    // Undefined behavior if the iterator is equal to 'end()'.

    BaggageRef ref() const;
    // Returns a BaggageRef object, containing references to the underlying
    // storage of the key:value pairs. References are invalided if the
    // associated SpanContext is destroyed or modified.

    BaggageIterator operator++(int);
    // Return a copy of this iterator, then post-increment this iterator to
    // point to the next key:value pair in the sequence.

    BaggageIterator& operator++();
    // Increment this iterator, pointing to the next key:value pair in the
    // sequence, then return a reference to this iterator.

    bool operator==(const BaggageIterator& other) const;
    // Returns true if this iterator points to the same key:value pair as
    // 'other', and false otherwise.

    bool operator!=(const BaggageIterator& other) const;
    // Returns true if this iterator points at a different key:value pair
    // than 'other', and false otherwise.

  private:
    const_iterator d_iterator;  // Traverses implementation storage
    ADAPTER        d_handler;   // Converts d_iterator to Baggage wrappers
};

// ----------------
// class BaggageImp
// ----------------

template <typename CHAR>
inline BaggageImp<CHAR>::BaggageImp() : m_key(), m_value()
{
}

template <typename CHAR>
inline BaggageImp<CHAR>::BaggageImp(const StringRefImp<CHAR>& key,
                                    const StringRefImp<CHAR>& value)
: m_key(key.data(), key.length()), m_value(value.data(), value.length())
{
}

template <typename CHAR>
inline std::basic_string<CHAR>&
BaggageImp<CHAR>::key()
{
    return m_key;
}

template <typename CHAR>
inline std::basic_string<CHAR>&
BaggageImp<CHAR>::value()
{
    return m_value;
}

template <typename CHAR>
inline const std::basic_string<CHAR>&
BaggageImp<CHAR>::key() const
{
    return m_key;
}

template <typename CHAR>
inline const std::basic_string<CHAR>&
BaggageImp<CHAR>::value() const
{
    return m_value;
}

template <typename CHAR>
BaggageImp<CHAR>* BaggageImp<CHAR>::operator->()
{
    return this;
}

template <typename CHAR>
const BaggageImp<CHAR>* BaggageImp<CHAR>::operator->() const
{
    return this;
}

// -------------------
// class BaggageRefImp
// -------------------

template <typename CHAR>
inline BaggageRefImp<CHAR>::BaggageRefImp() : m_key(), m_value()
{
}

template <typename CHAR>
inline BaggageRefImp<CHAR>::BaggageRefImp(const StringRefImp<CHAR>& key,
                                          const StringRefImp<CHAR>& value)
: m_key(key), m_value(value)
{
}

template <typename CHAR>
inline const StringRefImp<CHAR>&
BaggageRefImp<CHAR>::key() const
{
    return m_key;
}

template <typename CHAR>
inline const StringRefImp<CHAR>&
BaggageRefImp<CHAR>::value() const
{
    return m_value;
}

template <typename CHAR>
inline const BaggageRefImp<CHAR>* BaggageRefImp<CHAR>::operator->() const
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
    return d_handler.ref(d_iterator);
}

template <typename ADAPTER>
BaggageRef BaggageIterator<ADAPTER>::operator->() const
{
    return d_handler.ref(d_iterator);
}

template <typename ADAPTER>
Baggage BaggageIterator<ADAPTER>::narrow() const
{
    return d_handler.narrow(d_iterator);
}

template <typename ADAPTER>
BaggageWide BaggageIterator<ADAPTER>::wide() const
{
    return d_handler.wide(d_iterator);
}

template <typename ADAPTER>
BaggageRef BaggageIterator<ADAPTER>::ref() const
{
    return d_handler.ref(d_iterator);
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
