#ifndef INCLUDED_OPENTRACING_CARRIERS_H
#define INCLUDED_OPENTRACING_CARRIERS_H

#include <opentracing/span.h>
#include <opentracing/spancontext.h>

namespace opentracing {

class Writer {
  public:
    virtual int inject(const Span&)        = 0;
    virtual int inject(const SpanContext&) = 0;

  protected:
    Writer();
    Writer(const Writer&);
};

class Reader {
  public:
    int extract(SpanContext*) const = 0;

  protected:
    Reader();
    Reader(const Reader&);
};

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_CARRIERS_H
