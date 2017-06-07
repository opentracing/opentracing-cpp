#include <opentracing/noop.h>

namespace opentracing {
namespace {
class NoopSpanContext : public SpanContext {
 public:
  void ForeachBaggageItem(
      std::function<bool(const std::string& key, const std::string& value)> f)
      const override {}
};

class NoopSpan : public Span {
 public:
  NoopSpan(std::shared_ptr<const Tracer>&& tracer)
      : tracer_(std::move(tracer)) {}
  void FinishWithOptions(
      const FinishSpanOptions& finish_span_options) override {}
  void SetOperationName(const std::string& name) override {}
  void SetTag(const std::string& key, const Value& value) override {}
  void SetBaggageItem(const std::string& restricted_key,
                      const std::string& value) override {}
  std::string BaggageItem(const std::string& restricted_key) const override {
    return {};
  }
  const SpanContext& context() const override { return span_context_; }
  const Tracer& tracer() const override { return *tracer_; }

 private:
  std::shared_ptr<const Tracer> tracer_;
  NoopSpanContext span_context_;
};

class NoopTracer : public Tracer,
                   public std::enable_shared_from_this<NoopTracer> {
 public:
  std::unique_ptr<Span> StartSpanWithOptions(
      const std::string& operation_name,
      const StartSpanOptions& options) const override {
    return std::unique_ptr<Span>(new NoopSpan(shared_from_this()));
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
}  // anonymous namespace

std::shared_ptr<Tracer> make_noop_tracer() {
  return std::shared_ptr<Tracer>(new NoopTracer());
}
}  // namesapce opentracing
