#ifndef OPENTRACING_NOOP_H
#define OPENTRACING_NOOP_H

#include <opentracing/tracer.h>

namespace opentracing {
class NoopSpanContext : public SpanContext {
 public:
  void ForeachBaggageItem(
      std::function<bool(const std::string& key, const std::string& value)> f)
      const override {}
};

class NoopSpan : public Span {
 public:
  void Finish(const FinishSpanOptions& finish_span_options) override {}
  void SetOperationName(const std::string& name) override {}
  void SetTag(const std::string& key, const Value& value) override {}
  void SetBaggageItem(const std::string& restricted_key,
                              const std::string& value) override {}
  std::string BaggageItem(const std::string& restricted_key) const override {}
  const SpanContext& context() const { return span_context_; }

 private:
  NoopSpanContext span_context_;
};

class NoopTracer : public Tracer {
 public:
  std::unique_ptr<Span> StartSpan(
      const std::string& operation_name,
      const StartSpanOptions& options) const override {
    return std::unique_ptr<Span>(new NoopSpan());
  }

  bool Inject(const SpanContext& sc, CarrierFormat format,
              const CarrierWriter& writer) override {
    return true;
  }

  std::unique_ptr<SpanContext> Extract(CarrierFormat format,
                                       const CarrierReader& reader) override {
    return nullptr;
  }
};
} // namespace opentracing

#endif // OPENTRACING_NOOP_H
