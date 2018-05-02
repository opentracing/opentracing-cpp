#include "mock_span_context.h"

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
MockSpanContext& MockSpanContext::operator=(MockSpanContext&& other) noexcept {
  data_.trace_id = other.data_.trace_id;
  data_.span_id = other.data_.span_id;
  data_.baggage = std::move(other.data_.baggage);
  return *this;
}

void MockSpanContext::ForeachBaggageItem(
    std::function<bool(const std::string& key, const std::string& value)> f)
    const {
  std::lock_guard<std::mutex> lock_guard{baggage_mutex_};
  for (const auto& baggage_item : data_.baggage) {
    if (!f(baggage_item.first, baggage_item.second)) {
      return;
    }
  }
}

void MockSpanContext::CopyData(SpanContextData& data) const {
  data.trace_id = data_.trace_id;
  data.span_id = data_.span_id;
  std::lock_guard<std::mutex> lock_guard{baggage_mutex_};
  data.baggage = data_.baggage;
}

std::unique_ptr<SpanContext> MockSpanContext::Clone() const noexcept try {
  auto result = std::unique_ptr<MockSpanContext>{new MockSpanContext{}};
  CopyData(result->data_);
  return std::unique_ptr<SpanContext>{result.release()};
} catch (const std::exception& /*e*/) {
  return nullptr;
}
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
