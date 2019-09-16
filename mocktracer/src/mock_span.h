#ifndef OPENTRACING_MOCKTRACER_SPAN_H
#define OPENTRACING_MOCKTRACER_SPAN_H

#include <opentracing/mocktracer/tracer.h>
#include <atomic>
#include <mutex>
#include "mock_span_context.h"

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

class MockSpan : public Span {
 public:
  MockSpan(std::shared_ptr<const Tracer>&& tracer, Recorder* recorder,
           string_view operation_name, const StartSpanOptions& options);

  ~MockSpan() override;

  void FinishWithOptions(const FinishSpanOptions& options) noexcept override;

  void SetOperationName(string_view name) noexcept override;

  void SetTag(string_view key,
              const opentracing::Value& value) noexcept override;

  void Log(std::initializer_list<std::pair<string_view, Value>>
               fields) noexcept override;

  void Log(SystemTime timestamp,
           std::initializer_list<std::pair<string_view, Value>>
               fields) noexcept override;

  void Log(SystemTime timestamp,
           const std::vector<std::pair<string_view, Value>>&
               fields) noexcept override;

  void SetBaggageItem(string_view restricted_key,
                      string_view value) noexcept override;

  std::string BaggageItem(string_view restricted_key) const noexcept override;

  const SpanContext& context() const noexcept override { return span_context_; }

  const opentracing::Tracer& tracer() const noexcept override {
    return *tracer_;
  }

 private:
  std::shared_ptr<const Tracer> tracer_;
  Recorder* recorder_;
  MockSpanContext span_context_;
  SteadyTime start_steady_;

  std::atomic<bool> is_finished_{false};

  // Mutex protects data_
  std::mutex mutex_;
  SpanData data_;
};

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_SPAN_H
