#ifndef INCLUDED_OPENTRACING_TEST_SPANCONTEXT_H
#define INCLUDED_OPENTRACING_TEST_SPANCONTEXT_H

#include "unittest.h"

#include <opentracing/spancontext.h>
#include <opentracing/spancontext.h>  // test include guard

#include <map>
#include <string>

typedef std::multimap<std::string, std::string> TestBaggageContainer;

struct TestContextBaggageAdapter {
    typedef TestBaggageContainer::iterator       iterator;
    typedef TestBaggageContainer::const_iterator const_iterator;

    Baggage
    copy(const const_iterator& it) const
    {
        return Baggage(it->first, it->second);
    }
    BaggageRef
    ref(const const_iterator& it) const
    {
        return BaggageRef(it->first, it->second);
    }
};

class TestContextImpl
    : public GenericSpanContext<TestContextImpl, TestContextBaggageAdapter> {
  public:
    int
    setBaggageImp(const StringRef& key, const StringRef& baggage)
    {
        m_baggage.insert(TestBaggageContainer::value_type(
            std::string(key.data(), key.length()),
            std::string(baggage.data(), baggage.length())));

        return 0;
    }

    int
    getBaggageImp(const StringRef& key, std::string* const baggage) const
    {
        std::vector<std::string> out;

        getBaggageImp(key, &out);

        if (1u == out.size())
        {
            *baggage = out[0];
            return 0;
        }
        else
        {
            return 1;
        }
    }

    int
    getBaggageImp(const StringRef&                key,
                  std::vector<std::string>* const baggage) const
    {
        baggage->clear();

        const std::string mkey(key.data(), key.length());

        const std::pair<TestBaggageContainer::const_iterator,
                        TestBaggageContainer::const_iterator>
            range = m_baggage.equal_range(mkey);

        for (TestBaggageContainer::const_iterator it  = range.first,
                                                  end = range.second;
             it != end;
             ++it)
        {
            baggage->push_back(it->second);
        }

        return baggage->empty();
    }

    BaggageIterator
    baggageBeginImp() const
    {
        return BaggageIterator(m_baggage.begin());
    }

    BaggageIterator
    baggageEndImp() const
    {
        return BaggageIterator(m_baggage.end());
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
