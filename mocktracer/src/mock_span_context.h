#ifndef OPENTRACING_MOCKTRACER_SPAN_CONTEXT_H
#define OPENTRACING_MOCKTRACER_SPAN_CONTEXT_H

#include <opentracing/mocktracer/tracer.h>
#include <exception>
#include <mutex>
#include <string>
#include "propagation.h"

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

class MockSpan;

class MockSpanContext : public SpanContext {
 public:
  MockSpanContext() = default;

  MockSpanContext(SpanContextData&& data) noexcept : data_(std::move(data)) {}

  MockSpanContext(const MockSpanContext&) = delete;
  MockSpanContext(MockSpanContext&&) = delete;

  ~MockSpanContext() override = default;

  MockSpanContext& operator=(const MockSpanContext&) = delete;
  MockSpanContext& operator=(MockSpanContext&& other) noexcept;

  void ForeachBaggageItem(
      std::function<bool(const std::string& key, const std::string& value)> f)
      const override;

  std::string ToTraceID() const noexcept override try {
    return std::to_string(data_.trace_id);
  } catch (const std::exception& /*e*/) {
    return {};
  }

  std::string ToSpanID() const noexcept override try {
    return std::to_string(data_.span_id);
  } catch (const std::exception& /*e*/) {
    return {};
  }

  uint64_t trace_id() const noexcept { return data_.trace_id; }

  uint64_t span_id() const noexcept { return data_.span_id; }

  void CopyData(SpanContextData& data) const;

  template <class Carrier>
  expected<void> Inject(const PropagationOptions& propagation_options,
                        Carrier& writer) const {
    std::lock_guard<std::mutex> lock_guard{baggage_mutex_};
    return InjectSpanContext(propagation_options, writer, data_);
  }

  template <class Carrier>
  expected<bool> Extract(const PropagationOptions& propagation_options,
                         Carrier& reader) {
    std::lock_guard<std::mutex> lock_guard{baggage_mutex_};
    return ExtractSpanContext(propagation_options, reader, data_);
  }

  std::unique_ptr<SpanContext> Clone() const noexcept override;

 private:
  friend MockSpan;

  mutable std::mutex baggage_mutex_;
  SpanContextData data_;
};

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_SPAN_CONTEXT_H
