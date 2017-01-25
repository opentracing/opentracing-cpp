#ifndef INCLUDED_OPENTRACING_TEST_SPAN_H
#define INCLUDED_OPENTRACING_TEST_SPAN_H

#include "spancontext.h"
#include "unittest.h"

#include <opentracing/span.h>  // test include guard
#include <opentracing/span.h>

class TestSpanImpl : public GenericSpan<TestSpanImpl,
                                        TestContextImpl,
                                        TestContextBaggageAdapter> {
  public:
    const TestContextImpl *
    contextImp() const
    {
        TestContextImpl * context = new TestContextImpl();
        context->baggageMap() = m_context.baggageMap();
    }

    int
    setOperationImp(const StringRef&)
    {
        return 0;
    }

    int
    getBaggageImp(const StringRef& key, std::string* baggage) const
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
    getBaggageImp(const StringRef& key, std::vector<std::string>* baggage) const
    {
        baggage->clear();

        const std::string mkey(key.data(), key.length());

        const std::pair<TestBaggageContainer::const_iterator,
                        TestBaggageContainer::const_iterator>
            range = m_context.baggageMap().equal_range(mkey);

        for (TestBaggageContainer::const_iterator it  = range.first,
                                                  end = range.second;
             it != end;
             ++it)
        {
            baggage->push_back(it->second);
        }

        return baggage->empty();
    }
    int
    setBaggageImp(const StringRef& key, const StringRef& baggage)
    {
        m_context.baggageMap().insert(TestBaggageContainer::value_type(
            std::string(key.data(), key.length()),
            std::string(baggage.data(), baggage.length())));

        return 0;
    }

    int
    addReferenceImp(const TestContextImpl&, const SpanReferenceType::Value)
    {
        return 0;
    }

    int
    finishImp()
    {
        return 0;
    }

    int
    finishImp(const uint64_t)
    {
        return 0;
    }

    template <typename T>
    int
    tagImp(const StringRef&, const T&)
    {
        return 0;
    }

    template <typename T>
    int
    logImp(const StringRef&, const T&)
    {
        return 0;
    }

    template <typename T>
    int
    logImp(const StringRef&, const T&, const uint64_t)
    {
        return 0;
    }

    TestContextImpl m_context;
};

typedef GenericSpan<TestSpanImpl, TestContextImpl, TestContextBaggageAdapter>
    TestSpan;

#endif
