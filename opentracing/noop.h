#ifndef INCLUDED_OPENTRACING_NOOP_H
#define INCLUDED_OPENTRACING_NOOP_H

#include <opentracing/baggage.h>
#include <opentracing/config.h>
#include <opentracing/span.h>
#include <opentracing/spancontext.h>
#include <opentracing/stringref.h>
#include <opentracing/tracer.h>

#if HAVE_STDINT_H
#include <stdint.h>
#endif

// These are No-Op implementations of the GenericAdapter, GenericSpanContext,
// GenericSpan, and GenericTracer interfaces. These can be used to begin
// instrumenting applications in advanced of a concrete implementation
// being installed.

struct NoopContext;

struct NoopAdapter {
    typedef NoopContext*       iterator;
    typedef const NoopContext* const_iterator;

    BaggageRef
    ref()(const const_iterator& it) const
    {
        return BaggageRef();
    }
    Baggage
    narrow(const const_iterator& it) const
    {
        return Baggage();
    }
    BaggageWide
    wide(const const_iterator& it) const
    {
        return BaggageWide();
    }
};

struct NoopContext : public GenericSpanContext<NoopContext, NoopAdapter> {
    int
    setBaggageImp(const StringRef& key, const StringRef& baggage)
    {
        return 0;
    }
    int
    getBaggageImp(StringRef* const baggage, const StringRef& key)
    {
        return 1;
    }
    const_iterator
    beginImp() const
    {
        return const_iterator(this);
    }
    const_iterator
    endImp() const
    {
        return const_iterator(this);
    }
};

struct NoopSpan : public GenericSpan<NoopSpan, NoopContext, NoopAdapter> {
  public:
    NoopContext&
    contextImp()
    {
        return m_context;
    }

    const NoopContext&
    contextImp()
    {
        return m_context;
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
    NoopContext m_context;
};

struct NoopTracer
    : public GenericTracer<NoopTracer, NoopSpan, NoopContext, NoopAdapter> {
    Span*
    startImp(const StringRef&)
    {
        return NoopSpan();
    }
    Span*
    startImp(const StringRef&, const SpanContext&)
    {
        return NoopSpan();
    }
    template <typename ITER>
    Span*
    startImp(const StringRef&, const ITER&, const ITER&)
    {
        return NoopSpan();
    }
    Span*
    startImp(const StringRef&, const uint64_t)
    {
        return NoopSpan();
    }
    Span*
    startImp(const StringRef&, const SpanContext&, const uint64_t)
    {
        return NoopSpan();
    }
    template <typename ITER>
    Span*
    startImp(const StringRef&, const ITER&, const ITER&, const uint64_t)
    {
        return NoopSpan();
    }
    void cleanupImp(Span* const sp)
    {
        delete static_cast<NoopSpan>(sp);
    }
    template <typename CIMPL>
    int
    injectImp(GenericTextWriter<CIMPL>* const carrier,
              const SpanContext&              context) const
    {
        return 0;
    }
    template <typename CIMPL>
    int
    injectImp(GenericBinaryWriter<CIMPL>* const carrier,
              const SpanContext&                context) const
    {
        return 0;
    }
    template <typename CIMPL>
    int
    injectImp(GenericTextWriter<CIMPL, NoopContext>* const carrier,
              const SpanContext& context) const
    {
        return 0;
    }
    template <typename CIMPL>
    SpanContext*
    extractImp(const GenericTextReader<CIMPL>& carrier)
    {
        return NULL;
    }
    template <typename CIMPL>
    SpanContext*
    extractImp(const GenericBinaryReader<CIMPL>& carrier)
    {
        return NULL;
    }
    template <typename CIMPL>
    SpanContext*
    extractImp(const GenericBinaryReader<CIMPL, NoopContext>& carrier)
    {
        return NULL;
    }
    void cleanupImp(SpanContext* const sp)
    {
        delete static_cast<NoopContext>(sp);
    }
};

#endif
