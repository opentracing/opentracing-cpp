#ifndef INCLUDED_OPENTRACING_SPANOPTIONS_H
#define INCLUDED_OPENTRACING_SPANOPTIONS_H

// =============
// spanoptions.h
// =============
// class SpanRelationship   - Enumeration for Span relationship types
// class GenericSpanOptions - CRTP interface for SpanOptions

#include <opentracing/spancontext.h>

#include <opentracing/config.h>
#if HAVE_STDINT_H
#include <stdint.h>
#endif

namespace opentracing {

// ======================
// class SpanRelationship
// ======================
// Spans may reference zero or more other SpanContexts that are causally related.
// OpenTracing presently defines two types of references: ChildOf and
// FollowsFrom. Both reference types specifically model direct causal
// relationships between a child Span and a parent Span.
//
//  * ChildOf - A Span may be the ChildOf a parent Span. In a ChildOf reference,
//  			the parent Span depends on the child Span in some capacity.
//
//  * FollowsFrom - Some parent Spans do not depend in any way on the result of
//  				their child Spans. In these cases, we say merely that the
//  			 	child Span FollowsFrom the parent Span in a causal sense.

class SpanRelationship {
  public:
    enum Value
    {
        e_ChildOf     = 0,
        e_FollowsFrom = 1
    };
};

// ========================
// class GenericSpanOptions
// ========================
// This class defines the interface that clients can use to configure how a
// Span is created. These options are passed to the Tracer through the
// 'Tracer::start(const SpanOptions&)` method..

template <typename OPTIONS, typename CONTEXT, typename ADAPTER>
class GenericSpanOptions {
  public:
    typedef GenericSpanContext<CONTEXT, ADAPTER> SpanContext;

    void setOperation(const StringRef&);
    // Set the operation name to be used for any Span created with these
    // options.

    void setStartTime(const uint64_t);
    // Set the start time from this span. If start time is not supplied, the
    // default is to use the current wall-time.

    void addReference(const SpanRelationship::Value relationship,
                      const SpanContext&            context);
    // A new Span created with these options would have a 'relationship'
    // referenced added for 'context'.
};

// ------------------------
// class GenericSpanOptions
// ------------------------

template <typename OPTIONS, typename CONTEXT, typename ADAPTER>
inline void
GenericSpanOptions<OPTIONS, CONTEXT, ADAPTER>::setOperation(const StringRef& op)
{
    static_cast<OPTIONS*>(this)->setOperationImp(op);
}

template <typename OPTIONS, typename CONTEXT, typename ADAPTER>
inline void
GenericSpanOptions<OPTIONS, CONTEXT, ADAPTER>::setStartTime(const uint64_t tsp)
{
    static_cast<OPTIONS*>(this)->setStartTimeImp(tsp);
}

template <typename OPTIONS, typename CONTEXT, typename ADAPTER>
inline void
GenericSpanOptions<OPTIONS, CONTEXT, ADAPTER>::addReference(
    const SpanRelationship::Value rel, const SpanContext& context)
{
    const CONTEXT& contextImp = static_cast<const CONTEXT&>(context);
    static_cast<OPTIONS*>(this)->addReferenceImp(rel, contextImp);
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPANOPTIONS_H
