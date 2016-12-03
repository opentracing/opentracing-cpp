#include <opentracing/tracer.h>
#include <opentracing/span.h>

#include <stdexcept>
#include <cassert>


#if defined(OPENTRACING_USE_STD_ATOMIC) || defined(OPENTRACING_USE_BOOST_ATOMIC)
    #ifdef OPENTRACING_USE_STD_ATOMIC
        #include <atomic>
        #define TRACER_PTR_T std::atomic<Tracer*>
    #else
        #include <boost/atomic.hpp>
        #define TRACER_PTR_T boost::atomic<Tracer*>
    #endif

    #define SWAP_TRACER_PTR(ptr, value) ((value) = (ptr).exchange(value))
    #define RETRIEVE_TRACER_PTR(ptr) ((ptr).load())
#else
    #define TRACER_PTR_T Tracer *
    #define SWAP_TRACER_PTR(ptr, value) do { TRACER_PTR_T tmp(ptr); (ptr) = (value); (value) = tmp; } while (0)
    #define RETRIEVE_TRACER_PTR(ptr) (ptr)
#endif


namespace opentracing
{

StartSpanOptions::StartSpanOptions(const Span * parent_, const uint64_t & startTime_)
    : parent(parent_)
    , startTime(startTime_)
{
}

StartSpanOptions::StartSpanOptions(const uint64_t & startTime_)
    : parent(0)
    , startTime(startTime_)
{
}
    
Tracer::~Tracer()
{
}

Writer::~Writer()
{
}

TextMapWriter::~TextMapWriter()
{
}

Reader::~Reader()
{
}

TextMapReader::~TextMapReader()
{
}

TextMapReader::ReadCallback::~ReadCallback()
{
}

namespace {

class NoopSpan : public virtual Span
{
public:

    NoopSpan(const NoopTracer & noopTracer)
        : m_noopTracer(noopTracer)
    {
    }

    virtual ~NoopSpan()
    {
    }

    virtual const Tracer & getTracer() const
    {
        return m_noopTracer;
    }

private:

    const NoopTracer & m_noopTracer;
};

}

NoopTracer::~NoopTracer()
{
}

Span * NoopTracer::startSpan(const std::string & operationName, const StartSpanOptions & startSpanOptions) const
{
    (void)operationName;
    (void)startSpanOptions;
    return new NoopSpan(*this);
}

Tracer::Result NoopTracer::inject(const Span & sp, const Writer & writer, std::string & error) const
{
    (void)sp;
    (void)writer;
    (void)error;

    return Success;
}

Tracer::Result NoopTracer::join(Span ** sp, const std::string & operationName, const Reader & reader, std::string & error) const
{
    (void)sp;
    (void)operationName;
    (void)reader;
    (void)error;

    return ErrTraceNotFound;
}

namespace
{

NoopTracer defaultNoopTracer;
TRACER_PTR_T the_globalTracer(&defaultNoopTracer);

}

Tracer * globalTracer()
{
    return RETRIEVE_TRACER_PTR(the_globalTracer);
}

Tracer * initGlobalTracer(Tracer * tracer)
{
    if (!tracer)
    {
        throw std::runtime_error("tracer is NULL");
    }

    SWAP_TRACER_PTR(the_globalTracer, tracer);

    return tracer == &defaultNoopTracer ? NULL : tracer;
}

}
