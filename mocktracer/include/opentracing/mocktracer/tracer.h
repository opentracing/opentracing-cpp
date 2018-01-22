#ifndef OPENTRACING_MOCKTRACER_TRACER_H
#define OPENTRACING_MOCKTRACER_TRACER_H

#include <opentracing/mocktracer/recorder.h>
#include <opentracing/tracer.h>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

struct MockTracerOptions {
  std::unique_ptr<Recorder> recorder;
};

// MockTracer provides implements the OpenTracing Tracer API. It provides
// convenient access to finished spans in such a way as to support testing.
class MockTracer : public Tracer,
                   public std::enable_shared_from_this<MockTracer> {
 public:
  explicit MockTracer(MockTracerOptions&& options)
      : recorder_{std::move(options.recorder)} {}

  std::unique_ptr<Span> StartSpanWithOptions(
      string_view operation_name, const StartSpanOptions& options) const
      noexcept override;

  void Close() noexcept override;

  const std::vector<SpanData>& spans() const noexcept { return spans_; }

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
  std::mutex mutex_;
  std::vector<SpanData> spans_;
};

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_TRACER_H
