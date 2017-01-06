#ifndef INCLUDED_OPENTRACING_NOOP_H
#define INCLUDED_OPENTRACING_NOOP_H

// ======
// noop.h
// ======
// class NoopAdapter - No-op adapter for NoopContext baggage
// class NoopContext - No-op span context implementation
// class NoopSpan    - No-op span implementation
// class NoopTracer  - No-op tracer implementation
//
// -----
// No-Op
// -----
// This header provides a No-Op implementation for the generic OpenTracing
// classes. It can be installed globally in your application to begin
// instrumenting or testing the interface, but it has no side-effects, with
// one exception.
//
// In order to have client code behave the same when a concrete implementation
// is used, clients should be calling finish() and cleanup() appropriately.
// See span.h and tracer.h for details.
//
// See the specification for requirements of NoopTracers:
// https://github.com/opentracing/specification/blob/master/specification.md#nooptracer

#include <opentracing/baggage.h>
#include <opentracing/config.h>
#include <opentracing/span.h>
#include <opentracing/spancontext.h>
#include <opentracing/stringref.h>
#include <opentracing/tracer.h>

#if HAVE_STDINT_H
#include <stdint.h>
#endif

namespace opentracing{

class NoopAdapter;
class NoopContext;
class NoopSpan;
class NoopTracer;

// =================
// Class NoopAdapter
// =================
// The NoopAdapter has no container to iterator over. Regardless of
// what iterator is passed in, it will always return empty baggage
// objects or references.

class NoopAdapter {
  public:
    typedef NoopContext*       iterator;
    typedef const NoopContext* const_iterator;

    BaggageRef ref(const const_iterator& it) const;
    Baggage narrow(const const_iterator& it) const;
    BaggageWide wide(const const_iterator& it) const;
};

// =================
// Class NoopContext
// =================
// The NoopContext implements the GenericSpanContext interface. It
// provides no-op overloads for 'setBaggageImp' and 'getBaggageImp'.
//
// The baggage container iterator interface is faked by always
// using the address of the NoopContext itself for both the begin()
// and end() implementations. Since those pointers should always be
// equal, the iterator range will always be considered empty.
//
// If clients are using iterators correctly, the iterator should never
// be dereferenced, however, if they do dereference the BaggageIterator,
// they will be returned empty string references/copies.

class NoopContext : public GenericSpanContext<NoopContext, NoopAdapter> {
  public:
    int setBaggageImp(const StringRef& key, const StringRef& baggage);
    int getBaggageImp(StringRef* const baggage, const StringRef& key) const;
    const_iterator beginImp() const;
    const_iterator endImp() const;
};

// ==============
// Class NoopSpan
// ==============
// The NoopSpan implements the GenericSpan interface. It provides no-op
// overloads for all of the required Span behaviors.
//
// It owns a NoopContext to support the context() interface. Every method
// on the returned context() is also a no-op.

class NoopSpan : public GenericSpan<NoopSpan, NoopContext, NoopAdapter> {
  public:
    NoopContext&       contextImp();
    const NoopContext& contextImp() const;

    int finishImp();
    int finishImp(const uint64_t);

    int tagImp(const StringRef&, const StringRef&);
    int tagImp(const StringRef&, const bool);
    int tagImp(const StringRef&, const float);
    int tagImp(const StringRef&, const double);
    int tagImp(const StringRef&, const long double);
    int tagImp(const StringRef&, const char);
    int tagImp(const StringRef&, const signed char);
    int tagImp(const StringRef&, const unsigned char);
    int tagImp(const StringRef&, const wchar_t);
    int tagImp(const StringRef&, const int16_t);
    int tagImp(const StringRef&, const int32_t);
    int tagImp(const StringRef&, const int64_t);
    int tagImp(const StringRef&, const uint16_t);
    int tagImp(const StringRef&, const uint32_t);
    int tagImp(const StringRef&, const uint64_t);

    int logImp(const StringRef&, const StringRef&);
    int logImp(const StringRef&, const bool);
    int logImp(const StringRef&, const float);
    int logImp(const StringRef&, const double);
    int logImp(const StringRef&, const long double);
    int logImp(const StringRef&, const char);
    int logImp(const StringRef&, const signed char);
    int logImp(const StringRef&, const unsigned char);
    int logImp(const StringRef&, const wchar_t);
    int logImp(const StringRef&, const int16_t);
    int logImp(const StringRef&, const int32_t);
    int logImp(const StringRef&, const int64_t);
    int logImp(const StringRef&, const uint16_t);
    int logImp(const StringRef&, const uint32_t);
    int logImp(const StringRef&, const uint64_t);

    int logImp(const StringRef&, const StringRef&, const uint64_t);
    int logImp(const StringRef&, const bool, const uint64_t);
    int logImp(const StringRef&, const float, const uint64_t);
    int logImp(const StringRef&, const double, const uint64_t);
    int logImp(const StringRef&, const long double, const uint64_t);
    int logImp(const StringRef&, const char, const uint64_t);
    int logImp(const StringRef&, const signed char, const uint64_t);
    int logImp(const StringRef&, const unsigned char, const uint64_t);
    int logImp(const StringRef&, const wchar_t, const uint64_t);
    int logImp(const StringRef&, const int16_t, const uint64_t);
    int logImp(const StringRef&, const int32_t, const uint64_t);
    int logImp(const StringRef&, const int64_t, const uint64_t);
    int logImp(const StringRef&, const uint16_t, const uint64_t);
    int logImp(const StringRef&, const uint32_t, const uint64_t);
    int logImp(const StringRef&, const uint64_t, const uint64_t);

  private:
    NoopContext m_context;
};

// ================
// Class NoopTracer
// ================
// The NoopTracer implements the GenericTracer interface. It provides no-op
// overloads for all of the required Tracer behaviors.
//
// Clients can start spans and are still required to call 'cleanup' on
// returned Span and SpanContext pointers.

class NoopTracer
    : public GenericTracer<NoopTracer, NoopSpan, NoopContext, NoopAdapter> {
    // Note that Span' and SpanContext typedefs are provided by the base class
  public:
    Span* startImp(const StringRef&);
    Span* startImp(const StringRef&, const SpanContext&);

    template <typename ITER>
    Span* startImp(const StringRef&, const ITER&, const ITER&);

    Span* startImp(const StringRef&, const uint64_t);
    Span* startImp(const StringRef&, const SpanContext&, const uint64_t);

    template <typename ITER>
    Span* startImp(const StringRef&, const ITER&, const ITER&, const uint64_t);

    void cleanupImp(Span* const sp);

    template <typename CARRIER>
    int injectImp(GenericTextWriter<CARRIER>* const carrier,
                  const SpanContext&                context) const;

    template <typename CARRIER>
    int injectImp(GenericBinaryWriter<CARRIER>* const carrier,
                  const SpanContext&                  context) const;

    template <typename CARRIER>
    int injectImp(GenericWriter<CARRIER, NoopContext>* const carrier,
                  const SpanContext& context) const;

    template <typename CARRIER>
    SpanContext* extractImp(const GenericTextReader<CARRIER>& carrier);

    template <typename CARRIER>
    SpanContext* extractImp(const GenericBinaryReader<CARRIER>& carrier);

    template <typename CARRIER>
    SpanContext* extractImp(const GenericReader<CARRIER, NoopContext>& carrier);

    void cleanupImp(SpanContext* const sp);

  private:
    NoopSpan m_span;
};

// -----------------
// class NoopAdapter
// -----------------

inline BaggageRef
NoopAdapter::ref(const const_iterator&) const
{
    return BaggageRef();
}
inline Baggage
NoopAdapter::narrow(const const_iterator&) const
{
    return Baggage();
}
inline BaggageWide
NoopAdapter::wide(const const_iterator&) const
{
    return BaggageWide();
}

// -----------------
// class NoopContext
// -----------------

inline int
NoopContext::setBaggageImp(const StringRef&, const StringRef&)
{
    return 0;
}

inline int
NoopContext::getBaggageImp(StringRef* const, const StringRef&) const
{
    return 1;
}

inline NoopContext::const_iterator
NoopContext::beginImp() const
{
    return const_iterator(this);
}

inline NoopContext::const_iterator
NoopContext::endImp() const
{
    return const_iterator(this);
}

// -------------
// class NooSpan
// -------------

inline NoopContext&
NoopSpan::contextImp()
{
    return m_context;
}

inline const NoopContext&
NoopSpan::contextImp() const
{
    return m_context;
}

// --------------
// class NoopSpan
// --------------

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

inline int
NoopSpan::tagImp(const StringRef&, const StringRef&)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const bool)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const float)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const double)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const long double)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const char)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const signed char)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const unsigned char)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const wchar_t)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const int16_t)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const int32_t)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const int64_t)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const uint16_t)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const uint32_t)
{
    return 0;
}
inline int
NoopSpan::tagImp(const StringRef&, const uint64_t)
{
    return 0;
}

inline int
NoopSpan::logImp(const StringRef&, const StringRef&)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const bool)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const float)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const double)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const long double)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const char)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const signed char)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const unsigned char)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const wchar_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const int16_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const int32_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const int64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const uint16_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const uint32_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const uint64_t)
{
    return 0;
}

inline int
NoopSpan::logImp(const StringRef&, const StringRef&, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const bool, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const float, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const double, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const long double, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const char, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const signed char, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const unsigned char, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const wchar_t, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const int16_t, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const int32_t, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const int64_t, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const uint16_t, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const uint32_t, const uint64_t)
{
    return 0;
}
inline int
NoopSpan::logImp(const StringRef&, const uint64_t, const uint64_t)
{
    return 0;
}

// ----------------
// class NoopTracer
// ----------------

inline NoopTracer::Span*
NoopTracer::startImp(const StringRef&)
{
    return &m_span;
}

inline NoopTracer::Span*
NoopTracer::startImp(const StringRef&, const SpanContext&)
{
    return &m_span;
}

template <typename ITER>
inline NoopTracer::Span*
NoopTracer::startImp(const StringRef&, const ITER&, const ITER&)
{
    return &m_span;
}

inline NoopTracer::Span*
NoopTracer::startImp(const StringRef&, const uint64_t)
{
    return &m_span;
}

inline NoopTracer::Span*
NoopTracer::startImp(const StringRef&, const SpanContext&, const uint64_t)
{
    return &m_span;
}

template <typename ITER>
inline NoopTracer::Span*
NoopTracer::startImp(const StringRef&, const ITER&, const ITER&, const uint64_t)
{
    return &m_span;
}

inline void
NoopTracer::cleanupImp(Span* const)
{
}

template <typename CARRIER>
inline int
NoopTracer::injectImp(GenericTextWriter<CARRIER>* const,
                      const SpanContext&) const
{
    return 0;
}

template <typename CARRIER>
inline int
NoopTracer::injectImp(GenericBinaryWriter<CARRIER>* const,
                      const SpanContext&) const
{
    return 0;
}

template <typename CARRIER>
inline int
NoopTracer::injectImp(GenericWriter<CARRIER, NoopContext>* const,
                      const SpanContext&) const
{
    return 0;
}

template <typename CARRIER>
inline NoopTracer::SpanContext*
NoopTracer::extractImp(const GenericTextReader<CARRIER>&)
{
    return &m_span.context();
}

template <typename CARRIER>
inline NoopTracer::SpanContext*
NoopTracer::extractImp(const GenericBinaryReader<CARRIER>&)
{
    return &m_span.context();
}

template <typename CARRIER>
inline NoopTracer::SpanContext*
NoopTracer::extractImp(const GenericReader<CARRIER, NoopContext>&)
{
    return &m_span.context();
}

inline void
NoopTracer::cleanupImp(SpanContext* const)
{
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_NOOP_H
