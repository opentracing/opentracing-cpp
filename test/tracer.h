#ifndef INCLUDED_OPENTRACING_TEST_TRACER_H
#define INCLUDED_OPENTRACING_TEST_TRACER_H

#include "unittest.h"

#include <opentracing/tracer.h>  // test include guard
#include <opentracing/tracer.h>

#include "span.h"
#include "spancontext.h"

class TestTracerImpl : public GenericTracer<TestTracerImpl,
                                            TestSpanImpl,
                                            TestContextImpl,
                                            TestContextBaggageAdapter> {
  public:
    Span*
    startImp(const StringRef&)
    {
        return new TestSpanImpl();
    }

    Span*
    startImp(const StringRef&, const uint64_t)
    {
        return new TestSpanImpl();
    }

    Span*
    startImp(const StringRef&, const SpanContext&)
    {
        return new TestSpanImpl();
    }

    Span*
    startImp(const StringRef&, const SpanContext&, const uint64_t)
    {
        return new TestSpanImpl();
    }

    template <typename ITER>
    Span*
    startImp(const StringRef&, const ITER& pbegin, const ITER& pend)
    {
        for (ITER it = pbegin; it != pend; ++it)
        {
            const TestContextImpl * ctx = static_cast<TestContextImpl*>(&(*it));
            // Output is ignored, but I want to flex the compiler here. Let's force
            // a dereference to make sure the types are okay

            StringRef ref;
            ctx->getBaggageImp(&ref, "hello");
        }
        return new TestSpanImpl();
    }

    template <typename ITER>
    Span*
    startImp(const StringRef&, const ITER pbegin, const ITER pend, const uint64_t)
    {
        for (ITER it = pbegin; it != pend; ++it)
        {
            const TestContextImpl * ctx = static_cast<TestContextImpl*>(&(*it));

            StringRef ref;
            ctx->getBaggageImp(&ref, "hello");
        }

        return new TestSpanImpl();
    }

    template <typename CIMPL>
    int
    injectImp(GenericTextWriter<CIMPL>* const carrier,
              const TestContext&              context) const
    {
        const TestContextImpl& imp = static_cast<const TestContextImpl&>(context);

        std::vector<TextMapPair> pairs;
        pairs.reserve(imp.baggageMap().size());

        for (std::map<std::string, std::string>::const_iterator it =
                 imp.baggageMap().begin();
             it != imp.baggageMap().end();
             ++it)
        {
            pairs.push_back(TextMapPair(it->first, it->second));
        }

        return carrier->inject(pairs);
    }

    template <typename CIMPL>
    SpanContext*
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
            imp->baggageMap()[it->m_name] = it->m_value;
        }
        return imp;
    }

    template <typename CIMPL>
    int
    injectImp(GenericBinaryWriter<CIMPL>* const carrier,
              const TestContext&) const
    {
        // Context unused for test. Implementations should encode
        // the context for a wire protocol here, most likely
        const int deadbeef = 0xdeadbeef;
        return carrier->inject(&deadbeef, sizeof(deadbeef));
    }

    template <typename CIMPL>
    SpanContext*
    extractImp(const GenericBinaryReader<CIMPL>& carrier)
    {
        size_t written = 0;
        int output = 0;

        if (int rc = carrier.extract(&output, &written, sizeof(output)))
        {
            return NULL;
        }
        else if (output == 0xdeadbeef)
        {
            return new TestContextImpl();
        }
        else
        {
            return NULL;
        }
    }

    template <typename CIMPL>
    int
    injectImp(GenericWriter<CIMPL, TestContextImpl>* const carrier,
              const TestContext& context) const
    {
        return carrier->inject(static_cast<const TestContextImpl&>(context));
    }

    template <typename CIMPL>
    SpanContext*
    extractImp(const GenericReader<CIMPL, TestContextImpl>& carrier)
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
};

typedef GenericTracer<TestTracerImpl,
                      TestSpanImpl,
                      TestContextImpl,
                      TestContextBaggageAdapter>
    Tracer;

#endif
