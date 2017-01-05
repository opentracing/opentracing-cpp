#ifndef INCLUDED_OPENTRACING_BAGGAGE_H
#define INCLUDED_OPENTRACING_BAGGAGE_H

#include <opentracing/stringref.h>

namespace opentracing {

class BaggageRef {
  public:
    BaggageRef(const StringRef& name, const StringRef& value);

    const StringRef& name() const;
    const StringRef& value() const;

    const BaggageRef* operator->() const;

  private:
    StringRef m_name;
    StringRef m_value;
};

template <typename ADAPTER>
struct BaggageIterator {
  public:
    typedef typename ADAPTER::iterator       iterator;
    typedef typename ADAPTER::const_iterator const_iterator;

    BaggageIterator(const iterator& iter);
    BaggageIterator(const const_iterator& iter);

    BaggageRef operator*() const;
    BaggageRef operator->() const;

    BaggageIterator operator++(int);
    BaggageIterator operator++();

    bool operator==(const BaggageIterator& other) const;
    bool operator!=(const BaggageIterator& other) const;

  private:
    const_iterator d_iterator;
    ADAPTER        d_handler;
};

inline BaggageRef::BaggageRef(const StringRef& name, const StringRef& value)
: m_name(name), m_value(value)
{
}

inline const StringRef&
BaggageRef::name() const
{
    return m_name;
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
BaggageIterator<ADAPTER> BaggageIterator<ADAPTER>::operator++()
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
