#include <opentracing/mock_tracer.h>
#include <exception>

#include "mock_span_context.h"
#include "mock_span.h"

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

std::unique_ptr<Span> MockTracer::StartSpanWithOptions(
    string_view operation_name, const StartSpanOptions& options) const
    noexcept try {
  return std::unique_ptr<Span>{
      new MockSpan{shared_from_this(), *recorder_, operation_name, options}};
} catch (const std::exception& e) {
  return nullptr;
}

void MockTracer::Close() noexcept {}

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
