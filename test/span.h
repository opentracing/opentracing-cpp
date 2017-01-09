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
    TestContextImpl&
    contextImp()
    {
        return d_context;
    }

    const TestContextImpl&
    contextImp() const
    {
        return d_context;
    }

    int
    setOperationImp(const StringRef&)
    {
        return 0;
    }

    int
    addReferenceImp(const TestContextImpl&, const SpanRelationship::Value)
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

  private:
    TestContextImpl d_context;
};

typedef GenericSpan<TestSpanImpl, TestContextImpl, TestContextBaggageAdapter>
    TestSpan;

#endif
