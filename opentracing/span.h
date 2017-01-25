#ifndef INCLUDED_OPENTRACING_SPAN_H
#define INCLUDED_OPENTRACING_SPAN_H

#include <opentracing/stringref.h>

#include <string>
#include <stdint.h>

namespace opentracing {

class Span {
  public:
    virtual ~Span();

    virtual int setOperation(const StringRef& operation) = 0;

    virtual int finish()                   = 0;
    virtual int finish(const uint64_t tsp) = 0;

    virtual int tag(const StringRef& key, int16_t) = 0;
    virtual int tag(const StringRef& key, int32_t) = 0;
    virtual int tag(const StringRef& key, int64_t) = 0;

    virtual int tag(const StringRef& key, uint16_t) = 0;
    virtual int tag(const StringRef& key, uint32_t) = 0;
    virtual int tag(const StringRef& key, uint64_t) = 0;

    virtual int tag(const StringRef& key, signed char)   = 0;
    virtual int tag(const StringRef& key, unsigned char) = 0;
    virtual int tag(const StringRef& key, char)          = 0;

    virtual int tag(const StringRef& key, bool)   = 0;
    virtual int tag(const StringRef& key, float)  = 0;
    virtual int tag(const StringRef& key, double) = 0;

    template <typename T>
    int tag(const StringRef& key, const T& val);

    virtual int log(const StringRef& key, int16_t) = 0;
    virtual int log(const StringRef& key, int32_t) = 0;
    virtual int log(const StringRef& key, int64_t) = 0;

    virtual int log(const StringRef& key, uint16_t) = 0;
    virtual int log(const StringRef& key, uint32_t) = 0;
    virtual int log(const StringRef& key, uint64_t) = 0;

    virtual int log(const StringRef& key, signed char)   = 0;
    virtual int log(const StringRef& key, unsigned char) = 0;
    virtual int log(const StringRef& key, char)          = 0;

    virtual int log(const StringRef& key, bool)   = 0;
    virtual int log(const StringRef& key, float)  = 0;
    virtual int log(const StringRef& key, double) = 0;

    template <typename T>
    int log(const StringRef& key, const T& val);

    virtual int log(const StringRef& key, int16_t, uint64_t) = 0;
    virtual int log(const StringRef& key, int32_t, uint64_t) = 0;
    virtual int log(const StringRef& key, int64_t, uint64_t) = 0;

    virtual int log(const StringRef& key, uint16_t, uint64_t) = 0;
    virtual int log(const StringRef& key, uint32_t, uint64_t) = 0;
    virtual int log(const StringRef& key, uint64_t, uint64_t) = 0;

    virtual int log(const StringRef& key, signed char, uint64_t)   = 0;
    virtual int log(const StringRef& key, unsigned char, uint64_t) = 0;
    virtual int log(const StringRef& key, char, uint64_t)          = 0;

    virtual int log(const StringRef& key, bool, uint64_t)   = 0;
    virtual int log(const StringRef& key, float, uint64_t)  = 0;
    virtual int log(const StringRef& key, double, uint64_t) = 0;

    template <typename T>
    int log(const StringRef& key, const T& val, uint64_t);

    virtual int setBaggage(const StringRef& key, const StringRef& value) = 0;

    virtual int getBaggage(const StringRef& key,
                           std::string*     baggage) const = 0;

    virtual int getBaggage(const StringRef&          key,
                           std::vector<std::string>* baggage) const = 0;

    virtual int externalize(std::vector<TextMapPair> *) = 0;
    virtual int externalize(std::vector<char>* buf) = 0;

  protected:
    Span();
    Span(const Span&);
    // Protected to avoid direct construction
};

template<typename T>
int Span::tag(const StringRef& key, const T& val)
{
    std::ostringstream ss;
    ss << val;
    return log(key, ss.str());
}

template<typename T>
int Span::log(const StringRef& key, const T& val)
{
    std::ostringstream ss;
    ss << val;
    return log(key, ss.str());
}

template<typename T>
int Span::log(const StringRef& key, const T& val, uint64_t tsp)
{
    std::ostringstream ss;
    ss << val;
    return log(key, ss.str(), tsp);
}

}  // namespace opentracing
#endif  // INCLUDED_OPENTRACING_SPAN_H
