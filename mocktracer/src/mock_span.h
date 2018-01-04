#ifndef OPENTRACING_MOCK_SPAN_H
#define OPENTRACING_MOCK_SPAN_H

#include <opentracing/mock_tracer.h>
#include "mock_span_context.h"

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

class MockSpan : public Span {
 public:
  MockSpan(std::shared_ptr<const Tracer>&& tracer, Recorder& recorder,
           string_view operation_name, const StartSpanOptions& options);

  void FinishWithOptions(const FinishSpanOptions& options) noexcept override {}

  void SetOperationName(string_view name) noexcept override {
    data_.operation_name = name;
  }

  void SetTag(string_view key,
              const opentracing::Value& value) noexcept override {}

  void SetBaggageItem(string_view restricted_key,
                      string_view value) noexcept override {}

  std::string BaggageItem(string_view restricted_key) const noexcept override {
    return {};
   }

   void Log(std::initializer_list<std::pair<string_view, Value>>
                fields) noexcept override {}

   const SpanContext& context() const noexcept override {
     return span_context_;
  }

  const opentracing::Tracer& tracer() const noexcept override {
    return *tracer_;
  }

 private:
  std::shared_ptr<const Tracer> tracer_;
  Recorder& recorder_;
  MockSpanContext span_context_;
  SteadyTime start_steady_;
  SpanData data_;
};

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif // OPENTRACING_MOCK_SPAN_H
