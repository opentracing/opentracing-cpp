#include "in_memory_stream.h"

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
InMemoryBuffer::InMemoryBuffer(const char* data, size_t size) {
  // Data isn't modified so this is safe.
  auto non_const_data = const_cast<char*>(data);
  setg(non_const_data, non_const_data, non_const_data + size);
}

InMemoryStream::InMemoryStream(const char* data, size_t size)
    : InMemoryBuffer{data, size},
      std::istream{static_cast<std::streambuf*>(this)} {}
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
