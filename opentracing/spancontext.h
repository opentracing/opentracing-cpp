#ifndef INCLUDED_OPENTRACING_SPANCONTEXT_H
#define INCLUDED_OPENTRACING_SPANCONTEXT_H

#include <opentracing/baggage.h>
#include <opentracing/stringref.h>
#include <string>
#include <vector>
#include <opentracing/textpair.h>

namespace opentracing {

struct BaggageCallback {
  public:
    virtual void operator()(const opentracing::StringRef &key,
                            const opentracing::StringRef &value) = 0;
  protected:
    BaggageCallback() {}
    BaggageCallback(const BaggageCallback&) {}
    BaggageCallback& operator=(BaggageCallback&) {}
    // Protected to avoid direct construction

    virtual ~BaggageCallback() {}
};

template class SpanContext {
  public:
    virtual void forAllBaggage(BaggageCallback &cb) = 0;

    virtual int getBaggage(const StringRef &key,
                           std::string *    baggage) const = 0;

    virtual int getBaggage(const StringRef &         key,
                           std::vector<std::string> *baggage) const = 0;

    virtual int reset(const std::vector<TextPair>& textmap) = 0;
    virtual int reset(void * buf, size_t len) = 0;

    virtual int externalize(std::vector<TextPair> *) = 0;
    virtual int externalize(std::vector<char>* buf) = 0;

  protected:
    virtual ~SpanContext(){}
    SpanContext(){}
    SpanContext(const SpanContext &){}
    SpanContext & operator=(SpanContext &){}
    // Protected to avoid direct construction
};

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPANCONTEXT_H
