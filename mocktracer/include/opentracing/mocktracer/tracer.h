#ifndef OPENTRACING_MOCKTRACER_TRACER_H
#define OPENTRACING_MOCKTRACER_TRACER_H

#include <opentracing/mocktracer/recorder.h>
#include <opentracing/mocktracer/symbols.h>
#include <opentracing/tracer.h>
#include <map>
#include <memory>
#include <mutex>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

struct PropagationOptions {
  // Specifies what key to use when injecting and extracting span context.
  std::string propagation_key = "x-ot-span-context";

  // If inject_error_code is non-zero, MockTracer::Inject fails with
  // inject_error_code.
  std::error_code inject_error_code;

  // If extract_error_code is non-zero, MockTracer::Extract fails with
  // extract_error_code.
  std::error_code extract_error_code;
};

struct MockTracerOptions {
  // Recorder is sent spans when they are finished. If nullptr, all finished
  // spans are dropped.
  std::unique_ptr<Recorder> recorder;

  // PropagationOptions allows you to customize how the mocktracer's SpanContext
  // is propagated.
  PropagationOptions propagation_options;
};

// MockTracer provides implements the OpenTracing Tracer API. It provides
// convenient access to finished spans in such a way as to support testing.
class OPENTRACING_MOCK_TRACER_API MockTracer
    : public Tracer,
      public std::enable_shared_from_this<MockTracer> {
 public:
  explicit MockTracer(MockTracerOptions&& options);

  std::unique_ptr<Span> StartSpanWithOptions(
      string_view operation_name, const StartSpanOptions& options) const
      noexcept override;

  void Close() noexcept override;

  const std::vector<SpanData>& spans() const noexcept { return spans_; }

  using Tracer::Extract;
  using Tracer::Inject;

  expected<void> Inject(const SpanContext& sc,
                        std::ostream& writer) const override;

  expected<void> Inject(const SpanContext& sc,
                        const TextMapWriter& writer) const override;

  expected<void> Inject(const SpanContext& sc,
                        const HTTPHeadersWriter& writer) const override;

  expected<std::unique_ptr<SpanContext>> Extract(
      std::istream& reader) const override;

  expected<std::unique_ptr<SpanContext>> Extract(
      const TextMapReader& reader) const override;

  expected<std::unique_ptr<SpanContext>> Extract(
      const HTTPHeadersReader& reader) const override;

 private:
  std::unique_ptr<Recorder> recorder_;
  PropagationOptions propagation_options_;
  std::mutex mutex_;
  std::vector<SpanData> spans_;
};

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_TRACER_H
