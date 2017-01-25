#ifndef INCLUDED_OPENTRACING_HOT_H
#define INCLUDED_OPENTRACING_HOT_H

// -------------- Copied from noop.h --------------

#include <opentracing/baggage.h>
#include <opentracing/span.h>
#include <opentracing/spancontext.h>
#include <opentracing/spanoptions.h>
#include <opentracing/stringref.h>
#include <opentracing/tracer.h>
#include <stdint.h>

namespace opentracing {

template <typename T1, typename T2> class HotAdapter;
template <typename T1, typename T2> class HotContext;
template <typename T1, typename T2> class HotOptions;
template <typename T1, typename T2> class HotSpan;
template <typename T1, typename T2> class HotTracer;

template <typename T1, typename T2>
class HotAdapter {
    typedef HotContext<T1, T2> MyHotContext;
  public:
    typedef MyHotContext*       iterator;
    typedef const MyHotContext* const_iterator;

    BaggageRef ref(const const_iterator& it) const
    {
        static const char empty[] = "";
        return BaggageRef(empty, empty);
    }

    Baggage copy(const const_iterator& it) const
    {
        return Baggage("", "");
    }
};


/**
 * Note: VERY TRICKY. Pay attention to const-ness here. Span's context mustnn't
 * be const but `extract()`'s has to be const. To be properly thought/implemented.
 */
template <typename T1, typename T2>
class HotContext : public GenericSpanContext<HotContext<T1, T2>, HotAdapter<T1, T2> > {
    typedef typename T1::SpanContext SpanContext1;
    typedef typename T2::SpanContext SpanContext2;

    const SpanContext1 * ctx1; // Note: Might be NULL
    const SpanContext2 * ctx2; // Note: Might be NULL

    friend class HotTracer<T1, T2>;
    friend class HotSpan<T1, T2>;

    HotContext(const SpanContext1 * ctx1_, const SpanContext2 * ctx2_)
        : ctx1(ctx1_)
        , ctx2(ctx2_)
    {
    }

    HotContext()
    {
    }

  public:

    typedef GenericSpanContext<HotContext<T1, T2>, HotAdapter<T1, T2> > BaggageIterator;

    BaggageIterator baggageBeginImp() const
    {
        return baggageEndImp();
    }
    BaggageIterator baggageEndImp() const
    {
        return BaggageIterator(this);
    }

    int getBaggageImp(const StringRef&, std::string*) const
    {
        return 1;
    }
    int getBaggageImp(const StringRef&, std::vector<std::string>*) const
    {
        return 1;
    }
};

template <typename T1, typename T2>
class HotOptions
    : public GenericSpanOptions<HotOptions<T1, T2>, HotContext<T1, T2>, HotAdapter<T1, T2> > {
  public:
    int setOperationImp(const StringRef&)
    {
        return 0;
    }
    int setStartTimeImp(const uint64_t)
    {
        return 0;
    }
    int setReferenceImp(const SpanReferenceType::Value, const HotContext<T1, T2>&)
    {
        return 0;
    }

    template <typename T>
    int setTagImp(const StringRef&, const T&)
    {
        return 0;
    }
};

template <typename T1, typename T2>
class HotSpan : public GenericSpan<HotSpan<T1,T2>, HotContext<T1,T2>, HotAdapter<T1,T2> > {
    typedef typename T1::Span SpanT1;
    typedef typename T2::Span SpanT2;

    // Note: Might be NULL
    SpanT1 * span1;
    // Note: Might be NULL
    SpanT2 * span2;

    friend class HotTracer<T1, T2>;

    HotSpan(SpanT1 * span1_, SpanT2 * span2_)
        : span1(span1_)
        , span2(span2_)
    {
    }

  public:

    const HotContext<T1,T2>* contextImp() const
    {
        return &m_context;
    }

    int setOperationImp(const StringRef&)
    {
        return 0;
    }
    int setBaggageImp(const StringRef&, const StringRef&)
    {
        return 0;
    }

    int getBaggageImp(const StringRef&, std::string*) const
    {
        return 1;
    }
    int getBaggageImp(const StringRef&, std::vector<std::string>*) const
    {
        return 1;
    }

    int finishImp()
    {
        return 0;
    }
    int finishImp(const uint64_t)
    {
        return 0;
    }

    template <typename T>
    int tagImp(const StringRef&, const T&)
    {
        return 0;
    }

    template <typename T>
    int logImp(const StringRef&, const T&)
    {
        return 0;
    }

    template <typename T>
    int logImp(const StringRef&, const T&, const uint64_t)
    {
        return 0;
    }

    HotContext<T1,T2> m_context;
};

template <typename T1, typename T2>
class HotTracer : public GenericTracer<HotTracer<T1, T2>,
                                       HotSpan<T1, T2>,
                                       HotOptions<T1, T2>,
                                       HotContext<T1, T2>,
                                       HotAdapter<T1, T2> > {
    typedef GenericTracer<HotTracer<T1, T2>,
            HotSpan<T1, T2>,
            HotOptions<T1, T2>,
            HotContext<T1, T2>,
            HotAdapter<T1, T2> > Parent;

    T1 & t1;
    T2 & t2;
  public:

    HotTracer(T1& t1_, T2&t2_)
        : t1(t1_)
        , t2(t2_)
    {
    }

    static void installImp(HotTracer*tracer)
    {
        s_tracer = tracer;
    }
    static void uninstallImp()
    {
        s_tracer = 0;
    }
    static HotTracer* instanceImp()
    {
        return s_tracer;
    }

    HotOptions<T1,T2>* makeSpanOptionsImp()
    {
        return new HotOptions<T1,T2>();
    }

    HotSpan<T1,T2>* startImp(const StringRef&name)
    {
        typename T1::Span * sp1 = t1.start(name); // note: might be NULL
        typename T2::Span * sp2 = t2.start(name); // note: might be NULL

        return new HotSpan<T1,T2>(sp1, sp2);
    }
    HotSpan<T1,T2>* startImp(const HotOptions<T1,T2>& options)
    {
        typename T1::Span * sp1 = t1.start(options); // note: might be NULL
        typename T2::Span * sp2 = t2.start(options); // note: might be NULL

        return new HotSpan<T1,T2>(sp1, sp2);
    }

    template <typename CARRIER>
    int injectImp(CARRIER* carrier, const HotSpan<T1,T2> & span) const
    {
        // What happens if one of those two fails?
        if (span.span1)
        {
            t1.inject(carrier, *span.span1);
        }

        if (span.span2)
        {
            t2.inject(carrier, *span.span2);
        }

        return 0;
    }

    template <typename CARRIER>
    HotContext<T1,T2>* extractImp(const CARRIER& carrier)
    {
        const typename T1::SpanContext* ctx1 = t1.extract(carrier);

        const typename T2::SpanContext* ctx2 = t2.extract(carrier);

        return new HotContext<T1,T2>(ctx1, ctx2);
    }

    void cleanupImp(const HotOptions<T1,T2>* ob)
    {
        if (ob)
        {
            delete ob;
        }
    }
    void cleanupImp(const typename Parent::Span*ob)
    {
        if (ob)
        {
            delete ob;
        }
    }

    void cleanupImp(const HotContext<T1,T2>*ob)
    {
        if (ob)
        {
            delete ob;
        }
    }

  private:
    static HotTracer* s_tracer;
};

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_HOT_H
