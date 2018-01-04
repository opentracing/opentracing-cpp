#include <opentracing/mocktracer.h>
#include <exception>

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

  SpanContextData data_;
};

class MockSpan : public Span {
 public:
  MockSpan(std::shared_ptr<const Tracer>&& tracer, Recorder& recorder,
           string_view operation_name, const StartSpanOptions& options)
      : tracer_{std::move(tracer)}, recorder_{recorder} {
    data_.operation_name = operation_name;
  }

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
  SpanData data_;
};

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
