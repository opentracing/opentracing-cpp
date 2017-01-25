#ifndef INCLUDED_OPENTRACING_TEST_TRACER_H
#define INCLUDED_OPENTRACING_TEST_TRACER_H

#include "unittest.h"

#include <opentracing/tracer.h>  // test include guard
#include <opentracing/tracer.h>

#include "span.h"
#include "spancontext.h"

class TestOptionsImpl : public GenericSpanOptions<TestOptionsImpl,
                                                  TestContextImpl,
                                                  TestContextBaggageAdapter> {
  public:
    int
    setOperationImp(const StringRef&)
    {
        return 0;
    }

    int
    setStartTimeImp(const uint64_t)
    {
        return 0;
    }

    int
    setReferenceImp(const SpanReferenceType::Value, const TestContextImpl&)
    {
        return 0;
    }

    template <typename T>
    int
    setTagImp(const StringRef&, const T&)
    {
        return 0;
    }
};

class TestTracerImpl : public GenericTracer<TestTracerImpl,
                                            TestSpanImpl,
                                            TestOptionsImpl,
                                            TestContextImpl,
                                            TestContextBaggageAdapter> {
  public:
    static TestTracerImpl* s_tracer;

    static void
    installImp(TestTracerImpl* inst)
    {
        s_tracer = inst;
    }

    static TestTracerImpl*
    instanceImp()
    {
        return s_tracer;
    }

    static void
    uninstallImp()
    {
        s_tracer = 0;
    }

    TestOptionsImpl*
    makeSpanOptionsImp()
    {
        return new TestOptionsImpl();
    }


    TestSpanImpl*
    startImp(const StringRef&)
    {
        return new TestSpanImpl();
    }

    TestSpanImpl*
    startImp(const TestOptionsImpl&)
    {
        return new TestSpanImpl();
    }

    template <typename CARRIER_T>
    int
    injectImp(CARRIER_T* carrier, const TestSpanImpl& imp) const
    {
        return injectImp(carrier, imp.m_context);
    }

    template <typename CIMPL>
    int
    injectImp(GenericTextWriter<CIMPL>* carrier,
              const TestContextImpl&    imp) const
    {
        std::vector<TextMapPair> pairs;
        pairs.reserve(imp.baggageMap().size());

        for (TestBaggageContainer::const_iterator it = imp.baggageMap().begin();
             it != imp.baggageMap().end();
             ++it)
        {
            pairs.push_back(TextMapPair(it->first, it->second));
        }

        return carrier->inject(pairs);
    }

    template <typename CIMPL>
    int
    injectImp(GenericBinaryWriter<CIMPL>* carrier, const TestContext&) const
    {
        // Context unused for test. Implementations should encode
        // the context for a wire protocol here, most likely
        const int         deadbeef = 0xdeadbeef;
        std::vector<char> buf(sizeof(deadbeef));
        buf.resize(sizeof(deadbeef));
        std::memcpy(&buf[0], &deadbeef, sizeof(deadbeef));

        return carrier->inject(buf.data(), buf.size());
    }

    template <typename CIMPL>
    int
    injectImp(GenericWriter<CIMPL>*  carrier,
              const TestContextImpl& context) const
    {
        return carrier->inject(context);
    }

    template <typename CIMPL>
    TestContextImpl*
    extractImp(const GenericTextReader<CIMPL>& carrier) const
    {
        std::vector<TextMapPair> pairs;

        if (int rc = carrier.extract(&pairs))
        {
            return NULL;
        }

        TestContextImpl* imp = new TestContextImpl();

        for (std::vector<TextMapPair>::const_iterator it = pairs.begin();
             it != pairs.end();
             ++it)
        {
            imp->baggageMap().insert(
                TestBaggageContainer::value_type(it->m_key, it->m_value));
        }
        return imp;
    }

    template <typename CIMPL>
    TestContextImpl*
    extractImp(const GenericBinaryReader<CIMPL>& carrier)
    {
        const int expected = 0xdeadbeef;

        std::vector<char> buf;

        if (int rc = carrier.extract(&buf))
        {
            return NULL;
        }

        if (buf.size() == sizeof(expected) &&
            expected == *(reinterpret_cast<int*>(&buf[0])))
        {
            return new TestContextImpl();
        }
        else
        {
            return NULL;
        }
    }

    template <typename CIMPL>
    TestContextImpl*
    extractImp(const GenericReader<CIMPL>& carrier)
    {
        TestContextImpl* imp = new TestContextImpl;
        if (int rc = carrier.extract(imp))
        {
            delete imp;
            return NULL;
        }
        else
        {
            return imp;
        }
    }

    void
    cleanupImp(const TestOptionsImpl* opts)
    {
        delete opts;
    }

    void
    cleanupImp(const TestSpanImpl* sp)
    {
        delete sp;
    }

    void
    cleanupImp(const TestContextImpl* spc)
    {
        delete spc;
    }
};

typedef GenericSpanOptions<TestOptionsImpl,
                           TestContextImpl,
                           TestContextBaggageAdapter>
    SpanOptions;

typedef GenericTracer<TestTracerImpl,
                      TestSpanImpl,
                      TestOptionsImpl,
                      TestContextImpl,
                      TestContextBaggageAdapter>
    Tracer;

#endif
