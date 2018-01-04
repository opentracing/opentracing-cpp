#ifndef OPENTRACING_MOCK_SPAN_CONTEXT_H
#define OPENTRACING_MOCK_SPAN_CONTEXT_H

#include <opentracing/mock_tracer.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

class MockSpanContext : public SpanContext {
 public:
  MockSpanContext() = default;

  MockSpanContext(SpanContextData&& data) noexcept : data_{std::move(data)} {}

  void ForeachBaggageItem(
      std::function<bool(const std::string& key, const std::string& value)> f)
      const override {}

  uint64_t trace_id() const noexcept { return data_.trace_id; }

  uint64_t span_id() const noexcept { return data_.span_id; }
 private:
  SpanContextData data_;
};

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif // OPENTRACING_MOCK_SPAN_CONTEXT_H
