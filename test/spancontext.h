#ifndef INCLUDED_OPENTRACING_TEST_SPANCONTEXT_H
#define INCLUDED_OPENTRACING_TEST_SPANCONTEXT_H

#include "unittest.h"

#include <opentracing/spancontext.h>
#include <opentracing/spancontext.h>  // test include guard

#include <map>
#include <string>

typedef std::map<std::string, std::string> TestBaggageContainer;

struct TestContextBaggageAdapter {
    typedef TestBaggageContainer::iterator       iterator;
    typedef TestBaggageContainer::const_iterator const_iterator;

    Baggage narrow(const const_iterator& it) const
    {
        return Baggage(it->first, it->second);
    }
    BaggageWide wide(const const_iterator& it) const
    {
        std::wstring key;
        std::wstring value;

        test_widen(&key, it->first);
        test_widen(&value, it->second);

        return BaggageWide(key, value);
    }
    BaggageRef ref(const const_iterator& it) const
    {
        return BaggageRef(it->first, it->second);
    }
};

class TestContextImpl
    : public GenericSpanContext<TestContextImpl, TestContextBaggageAdapter> {
  public:
    typedef GenericSpanContext<TestContextImpl, TestContextBaggageAdapter> Base;

    int
    setBaggageImp(const StringRef& key, const StringRef& baggage)
    {
        m_baggage[std::string(key.data(), key.length())] =
            std::string(baggage.data(), baggage.length());

        return 0;
    }

    int
    getBaggageImp(StringRef* const baggage, const StringRef& key) const
    {
        const std::string mkey(key.data(), key.length());

        const TestBaggageContainer::const_iterator it = m_baggage.find(mkey);

        if (m_baggage.end() != it)
        {
            baggage->reset(it->second.data(), it->second.length());
            return 0;
        }
        else
        {
            return 1;
        }
    }

    Base::const_iterator
    beginImp() const
    {
        return Base::const_iterator(m_baggage.begin());
    }

    Base::const_iterator
    endImp() const
    {
        return Base::const_iterator(m_baggage.end());
    }

    TestBaggageContainer&
    baggageMap()
    {
        return m_baggage;
    }

    const TestBaggageContainer&
    baggageMap() const
    {
        return m_baggage;
    }

  private:
    TestBaggageContainer m_baggage;
};

typedef GenericSpanContext<TestContextImpl, TestContextBaggageAdapter>
    TestContext;

#endif  // INCLUDED_OPENTRACING_TEST_SPANCONTEXT_H
