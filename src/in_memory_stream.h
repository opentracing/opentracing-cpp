#pragma once

#include <opentracing/version.h>
#include <istream>
#include <streambuf>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
// Class to allow construction of an istream from constant memory without
// copying. See https://stackoverflow.com/a/13059195/4447365.
class InMemoryBuffer : public std::streambuf {
 public:
  InMemoryBuffer(const char* data, size_t size);
};

class InMemoryStream : virtual public InMemoryBuffer, public std::istream {
 public:
  InMemoryStream(const char* data, size_t size);
};
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
