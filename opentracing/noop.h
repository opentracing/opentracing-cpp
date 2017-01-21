#ifndef INCLUDED_OPENTRACING_NOOP_H
#define INCLUDED_OPENTRACING_NOOP_H

// ======
// noop.h
// ======
// class NoopAdapter - No-op adapter for NoopContext baggage
// class NoopContext - No-op span context implementation
// class NoopOptions - No-op span options implementation
// class NoopSpan    - No-op span implementation
// class NoopTracer  - No-op tracer implementation
//
// -----
// No-Op
// -----
// This header provides a No-Op implementation for the generic OpenTracing
// classes. It can be installed globally in your application to begin
// instrumenting or testing the interface; it has no-side effects.
//
// In order to have client code behave the same when a concrete implementation
// is used, clients should be calling finish() and cleanup() appropriately.
//
// See the specification for requirements of NoopTracers:
// https://github.com/opentracing/specification/blob/master/specification.md#nooptracer

#include <opentracing/baggage.h>
#include <opentracing/span.h>
#include <opentracing/spancontext.h>
#include <opentracing/spanoptions.h>
#include <opentracing/stringref.h>
#include <opentracing/tracer.h>
#include <stdint.h>

namespace opentracing {

class NoopAdapter;
class NoopContext;
class NoopOptions;
class NoopSpan;
class NoopTracer;

// =================
// class NoopAdapter
// =================
// The NoopAdapter has no container to iterator over. Regardless of what
// iterator is passed in, it will always return empty baggage objects or
// references.

class NoopAdapter {
  public:
    typedef NoopContext*       iterator;
    typedef const NoopContext* const_iterator;

    BaggageRef ref(const const_iterator& it) const;
    Baggage copy(const const_iterator& it) const;
};

// =================
// class NoopContext
// =================
// The NoopContext implements the GenericSpanContext interface.
//
// The baggage iterator interface is faked by always using the address of the
// encompassing NoopContext for both the baggageBeginImp() and baggageEndImp()
// calls. Since those pointers should always be equal for the same SpanContext,
// the iterator range will always be considered empty.
//
// If clients are using iterators correctly, the iterator should never
// be dereferenced, however, if they do dereference the BaggageIterator,
// they will be returned empty string references/copies.

class NoopContext : public GenericSpanContext<NoopContext, NoopAdapter> {
  public:
    BaggageIterator baggageBeginImp() const;
    BaggageIterator baggageEndImp() const;

    int getBaggageImp(const StringRef&, std::string*) const;
    int getBaggageImp(const StringRef&, std::vector<std::string>*) const;
};

// =================
// class NoopOptions
// =================
// NoopOptions implements the GenericSpanOptions interface. It holds
// onto no references, nor does it manipulate the start time or operation
// names; it does not need to.

class NoopOptions
    : public GenericSpanOptions<NoopOptions, NoopContext, NoopAdapter> {
  public:
    int setOperationImp(const StringRef&);
    int setStartTimeImp(const uint64_t);
    int setReferenceImp(const SpanReferenceType::Value, const NoopContext&);

    template <typename T>
    int setTagImp(const StringRef&, const T&);
};

// ==============
// class NoopSpan
// ==============
// The NoopSpan implements the GenericSpan interface. It provides no-op
// overloads for all of the required Span behaviors.
//
// It owns a NoopContext to support the context() interface. Every method
// on the returned context() is also a no-op.

class NoopSpan : public GenericSpan<NoopSpan, NoopContext, NoopAdapter> {
  public:
    const NoopContext* contextImp() const;

    int setOperationImp(const StringRef&);
    int setBaggageImp(const StringRef&, const StringRef&);

    int getBaggageImp(const StringRef&, std::string*) const;
    int getBaggageImp(const StringRef&, std::vector<std::string>*) const;

    int finishImp();
    int finishImp(const uint64_t);

    template <typename T>
    int tagImp(const StringRef&, const T&);

    template <typename T>
    int logImp(const StringRef&, const T&);

    template <typename T>
    int logImp(const StringRef&, const T&, const uint64_t);

    NoopContext m_context;
};

// ================
// class NoopTracer
// ================
// The NoopTracer implements the GenericTracer interface. It provides no-op
// overloads for all of the required Tracer behaviors.
//
// Although clients can 'start()' spans or 'makeSpanOptions()', there are
// no allocations made. Clients should call 'finish()'/'cleanup()', however,
// this implementation has no side effects of them doing so. Inject/extract
// can be called, but there are no side effects; the noop tracer will
// never add itself to carrier objects, but will always return a valid
// pointer to a context.

class NoopTracer : public GenericTracer<NoopTracer,
                                        NoopSpan,
                                        NoopOptions,
                                        NoopContext,
                                        NoopAdapter> {
  public:
    static void        installImp(NoopTracer*);
    static void        uninstallImp();
    static NoopTracer* instanceImp();

    NoopOptions* makeSpanOptionsImp();

    NoopSpan* startImp(const StringRef&);
    NoopSpan* startImp(const NoopOptions&);

    template <typename CARRIER, typename T>
    int injectImp(CARRIER* carrier, const T&) const;

    template <typename CARRIER>
    NoopContext* extractImp(const CARRIER& carrier);

    void cleanupImp(const NoopOptions*);
    void cleanupImp(const Span*);
    void cleanupImp(const NoopContext*);

  private:
    static NoopTracer* s_tracer;

    NoopOptions m_opts;
    NoopSpan    m_span;
};

// -----------------
// class NoopAdapter
// -----------------

inline BaggageRef
NoopAdapter::ref(const const_iterator&) const
{
    static const char empty[] = "";
    return BaggageRef(empty, empty);
}

inline Baggage
NoopAdapter::copy(const const_iterator&) const
{
    return Baggage("", "");
}

// -----------------
// class NoopContext
// -----------------

inline NoopContext::BaggageIterator
NoopContext::baggageBeginImp() const
{
    return baggageEndImp();
}

inline NoopContext::BaggageIterator
NoopContext::baggageEndImp() const
{
    return BaggageIterator(this);
}

inline int
NoopContext::getBaggageImp(const StringRef&, std::string*) const
{
    return 1;
}

inline int
NoopContext::getBaggageImp(const StringRef&, std::vector<std::string>*) const
{
    return 1;
}

// -----------------
// class NoopOptions
// -----------------

inline int
NoopOptions::setOperationImp(const StringRef&)
{
    return 0;
}

inline int
NoopOptions::setStartTimeImp(const uint64_t)
{
    return 0;
}

inline int
NoopOptions::setReferenceImp(const SpanReferenceType::Value, const NoopContext&)
{
    return 0;
}

template <typename T>
int
NoopOptions::setTagImp(const StringRef&, const T&)
{
    return 0;
}

// -------------
// class NooSpan
// -------------

inline const NoopContext*
NoopSpan::contextImp() const
{
    return &m_context;
}

// --------------
// class NoopSpan
// --------------

inline int
NoopSpan::setOperationImp(const StringRef&)
{
    return 0;
}

inline int
NoopSpan::setBaggageImp(const StringRef&, const StringRef&)
{
    return 0;
}

inline int
NoopSpan::getBaggageImp(const StringRef&, std::string*) const
{
    return 1;
}

inline int
NoopSpan::getBaggageImp(const StringRef&, std::vector<std::string>*) const
{
    return 1;
}

inline int
NoopSpan::finishImp()
{
    return 0;
}

inline int
NoopSpan::finishImp(const uint64_t)
{
    return 0;
}

template <typename T>
int
NoopSpan::tagImp(const StringRef&, const T&)
{
    return 0;
}

template <typename T>
int
NoopSpan::logImp(const StringRef&, const T&)
{
    return 0;
}

template <typename T>
int
NoopSpan::logImp(const StringRef&, const T&, const uint64_t)
{
    return 0;
}

// ----------------
// class NoopTracer
// ----------------

inline void
NoopTracer::installImp(NoopTracer* tracer)
{
    s_tracer = tracer;
}

inline NoopTracer*
NoopTracer::instanceImp()
{
    return s_tracer;
}

inline void
NoopTracer::uninstallImp()
{
    s_tracer = 0;
}

inline NoopOptions*
NoopTracer::makeSpanOptionsImp()
{
    return &m_opts;
}

inline NoopSpan*
NoopTracer::startImp(const StringRef&)
{
    return &m_span;
}

inline NoopSpan*
NoopTracer::startImp(const NoopOptions&)
{
    return &m_span;
}

template <typename CARRIER, typename T>
int
NoopTracer::injectImp(CARRIER*, const T&) const
{
    return 0;
}

template <typename CARRIER>
NoopContext*
NoopTracer::extractImp(const CARRIER&)
{
    return &m_span.m_context;
}

inline void
NoopTracer::cleanupImp(const NoopOptions*)
{
}

inline void
NoopTracer::cleanupImp(const Span*)
{
}

inline void
NoopTracer::cleanupImp(const NoopContext*)
{
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_NOOP_H
