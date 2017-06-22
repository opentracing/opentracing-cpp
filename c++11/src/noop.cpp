#include <opentracing/noop.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace {
class NoopSpanContext : public SpanContext {
 public:
  void ForeachBaggageItem(
      std::function<bool(const std::string& key,
                         const std::string& value)> /*f*/) const override {}
};

class NoopSpan : public Span {
 public:
  explicit NoopSpan(std::shared_ptr<const Tracer>&& tracer) noexcept
      : tracer_(std::move(tracer)) {}
  void FinishWithOptions(
      const FinishSpanOptions& /*finish_span_options*/) noexcept override {}
  void SetOperationName(StringRef /*name*/) noexcept override {}
  void SetTag(StringRef /*key*/, const Value& /*value*/) noexcept override {}
  void SetBaggageItem(StringRef /*restricted_key*/,
                      StringRef /*value*/) noexcept override {}
  std::string BaggageItem(StringRef /*restricted_key*/) const
      noexcept override {
    return {};
  }
  void Log(std::initializer_list<std::pair<StringRef, Value>>
           /*fields*/) noexcept override {}
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
      StringRef /*operation_name*/, const StartSpanOptions& /*options*/) const
      noexcept override {
    return std::unique_ptr<Span>(new (std::nothrow)
                                     NoopSpan(shared_from_this()));
  }

  Expected<void> Inject(const SpanContext& /*sc*/,
                        const TextMapWriter& /*writer*/) const override {
    return {};
  }

  Expected<void> Inject(const SpanContext& /*sc*/,
                        const HTTPHeadersWriter& /*writer*/) const override {
    return {};
  }

  Expected<std::unique_ptr<SpanContext>> Extract(
      const TextMapReader& /*reader*/) const override {
    return std::unique_ptr<SpanContext>(nullptr);
  }
  Expected<std::unique_ptr<SpanContext>> Extract(
      const HTTPHeadersReader& /*reader*/) const override {
    return std::unique_ptr<SpanContext>(nullptr);
  }
};
}  // anonymous namespace

std::shared_ptr<Tracer> make_noop_tracer() noexcept {
  return std::shared_ptr<Tracer>(new (std::nothrow) NoopTracer());
}
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
