#include <memory>
#include <cassert>

template <typename Tracer>
class Cpp11Tracer
{
    Tracer& tracer;

    Cpp11Tracer(Tracer& tracer_)
        : tracer(tracer_)
    {
    }

public:
    typedef typename Tracer::Span Span;
    typedef typename Tracer::SpanContext SpanContext;

    template <typename Args>
    inline std::shared_ptr<Span> start(Args & args)
    {
        return std::shared_ptr<Span>(tracer.start(args), [this](Span *pi) { tracer.cleanup(pi); });
    }

    template <typename CARRIER>
    int inject(CARRIER* carrier, const Span& span) const
    {
        return tracer.inject(carrier, span);
    }

    template <typename CARRIER>
    int inject(CARRIER* carrier, const SpanContext& context) const
    {
        return tracer.inject(carrier, context);
    }

    template <typename CARRIER>
    std::shared_ptr<const SpanContext> extract(const CARRIER& carrier)
    {
        return std::shared_ptr<const SpanContext>(tracer.extract(carrier), [this](const SpanContext *pi) { tracer.cleanup(pi); });
    }

    static Cpp11Tracer instance()
    {
        Tracer * tracer = Tracer::instance();
        return Cpp11Tracer(*tracer);
    }
};
