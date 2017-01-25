#ifndef INCLUDED_OPENTRACING_SPANOPTIONS_H
#define INCLUDED_OPENTRACING_SPANOPTIONS_H

#include <opentracing/spancontext.h>
#include <stdint.h>

namespace opentracing {

// =======================
// class SpanReferenceType
// =======================
// Spans may reference zero or more other SpanContexts that are causally
// related.
// OpenTracing presently defines two types of references: ChildOf and
// FollowsFrom. Both reference types specifically model direct causal
// relationships between a child Span and a parent Span.
//
//  * e_ChildOf - A Span may be the ChildOf a parent Span. In a ChildOf
//  reference,
//                the parent Span depends on the child Span in some capacity.
//
//  * e_FollowsFrom - Some parent Spans do not depend in any way on the result
//  of
//                    their child Spans. In these cases, we say merely that the
//                    child Span FollowsFrom the parent Span in a causal sense.

class SpanReferenceType {
  public:
    enum Value
    {
        e_ChildOf     = 0,
        e_FollowsFrom = 1
    };
};

class SpanOptions {
  public:
    virtual int setOperation(const StringRef&) = 0;

    virtual int setStartTime(const uint64_t) = 0;

    virtual int setReference(const SpanReferenceType::Value relationship,
                             const SpanContext&             context) = 0;

    virtual int setTag(const StringRef& key, int16_t) = 0;
    virtual int setTag(const StringRef& key, int32_t) = 0;
    virtual int setTag(const StringRef& key, int64_t) = 0;

    virtual int setTag(const StringRef& key, uint16_t) = 0;
    virtual int setTag(const StringRef& key, uint32_t) = 0;
    virtual int setTag(const StringRef& key, uint64_t) = 0;

    virtual int setTag(const StringRef& key, signed char)   = 0;
    virtual int setTag(const StringRef& key, unsigned char) = 0;
    virtual int setTag(const StringRef& key, char)          = 0;

    virtual int setTag(const StringRef& key, bool)   = 0;
    virtual int setTag(const StringRef& key, float)  = 0;
    virtual int setTag(const StringRef& key, double) = 0;

    template <typename T>
    int setTag(const StringRef& key, const T& val);

  protected:
    SpanOptions() {}
    SpanOptions(const SpanOptions&) {}
    SpanOptions& operator=(const SpanOptions&) {}
    virtual ~SpanOptions() {};
};

template <typename T>
int
SpanOptions::setTag(const StringRef& key, const T& val)
{
    std::ostringstream ss;
    ss << val;
    return setTag(key, ss.str());
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPANOPTIONS_H
