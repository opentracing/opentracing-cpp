#ifndef INCLUDED_OPENTRACING_TEST_SPAN_H
#define INCLUDED_OPENTRACING_TEST_SPAN_H

#include "unittest.h"
#include "spancontext.h"

#include <opentracing/span.h>  // test include guard
#include <opentracing/span.h>

class TestSpanImpl : public GenericSpan<TestSpanImpl,
                                        TestContextImpl,
                                        TestContextBaggageAdapter> {
  public:
    TestContextImpl& contextImp() {
        return d_context;
    }

    const TestContextImpl& contextImp() const {
        return d_context;
    }

    int
    finishImp()
    {
        return 0;
    }

    int
    finishImp(const uint64_t )
    {
        return 0;
    }

    int
    tagImp(const StringRef&, const bool)
    {
        return 0;
    }

    int
    tagImp(const StringRef&, const float)
    {
        return 0;
    }

    int
    tagImp(const StringRef&, const double)
    {
        return 0;
    }

    int
    tagImp(const StringRef&, const StringRef&)
    {
        return 0;
    }

    int
    tagImp(const StringRef&, const int16_t)
    {
        return 0;
    }
    int
    tagImp(const StringRef&, const int32_t)
    {
        return 0;
    }
    int
    tagImp(const StringRef&, const int64_t)
    {
        return 0;
    }
    int
    tagImp(const StringRef&, const uint16_t)
    {
        return 0;
    }

    int
    tagImp(const StringRef&, const uint32_t)
    {
        return 0;
    }

    int
    tagImp(const StringRef&, const uint64_t)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const bool)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const float)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const double)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const StringRef&)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const int16_t)
    {
        return 0;
    }
    int
    logImp(const StringRef&, const int32_t)
    {
        return 0;
    }
    int
    logImp(const StringRef&, const int64_t)
    {
        return 0;
    }
    int
    logImp(const StringRef&, const uint16_t)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const uint32_t)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const uint64_t)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const bool, const uint64_t)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const float, const uint64_t)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const double, const uint64_t)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const StringRef&, const uint64_t)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const int16_t, const uint64_t)
    {
        return 0;
    }
    int
    logImp(const StringRef&, const int32_t, const uint64_t)
    {
        return 0;
    }
    int
    logImp(const StringRef&, const int64_t, const uint64_t)
    {
        return 0;
    }
    int
    logImp(const StringRef&, const uint16_t, const uint64_t)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const uint32_t, const uint64_t)
    {
        return 0;
    }

    int
    logImp(const StringRef&, const uint64_t, const uint64_t)
    {
        return 0;
    }

  private:
    TestContextImpl d_context;
};

typedef GenericSpan<TestSpanImpl, TestContextImpl, TestContextBaggageAdapter>
    TestSpan;

#endif
