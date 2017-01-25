#ifndef INCLUDED_OPENTRACING_TEXTPAIR_H
#define INCLUDED_OPENTRACING_TEXTPAIR_H

#include <opentracing/stringref.h>

namespace opentracing {

class TextPair {
  public:
    TextPair();
    TextPair(const StringRef& name, const StringRef& value);

    std::string m_key;
    std::string m_value;
};
}
#endif
