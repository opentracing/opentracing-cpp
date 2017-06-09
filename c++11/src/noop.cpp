#include <opentracing/noop.h>

namespace opentracing {
inline namespace OPENTRACING_VERSION_NAMESPACE {
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
      const FinishSpanOptions& finish_span_options) noexcept override {}
  void SetOperationName(StringRef name) override {}
  void SetTag(StringRef key, const Value& value) override {}
  void SetBaggageItem(StringRef restricted_key, StringRef value) override {}
  std::string BaggageItem(StringRef restricted_key) const override {
    return {};
  }
  const SpanContext& context() const noexcept override { return span_context_; }
  const Tracer& tracer() const noexcept override { return *tracer_; }

 private:
  std::shared_ptr<const Tracer> tracer_;
  NoopSpanContext span_context_;
};

class NoopTracer : public Tracer,
                   public std::enable_shared_from_this<NoopTracer> {
 public:
  std::unique_ptr<Span> StartSpanWithOptions(
      StringRef operation_name,
      const StartSpanOptions& options) const override {
    return std::unique_ptr<Span>(new NoopSpan(shared_from_this()));
  }

  Expected<void, std::string> Inject(
      const SpanContext& sc, CarrierFormat format,
      const CarrierWriter& writer) const override {
    return {};
  }

  std::unique_ptr<SpanContext> Extract(
      CarrierFormat format, const CarrierReader& reader) const override {
    return nullptr;
  }
};
}  // anonymous namespace

std::shared_ptr<Tracer> make_noop_tracer() noexcept {
  return std::shared_ptr<Tracer>(new (std::nothrow) NoopTracer());
}
}  // namespace OPENTRACING_VERSION_NAMESPACE
}  // namesapce opentracing
