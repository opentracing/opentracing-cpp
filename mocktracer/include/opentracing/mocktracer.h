#ifndef OPENTRACING_MOCKTRACER_H
#define OPENTRACING_MOCKTRACER_H

#include <opentracing/tracer.h>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
struct LogRecordData {
  SystemTime timestamp;
  std::vector<std::pair<std::string, Value>> fields;
};

struct SpanContextData {
  int64_t trace_id;
  int64_t span_id;
  std::unordered_map<std::string, std::string> baggage;
};

struct SpanData {
  SpanContextData span_context;
  std::vector<int64_t> parent_span_ids;
  std::vector<int64_t> follows_from_span_ids;
  std::string operation_name;
  SystemTime start_timestamp;
  SteadyClock::duration duration;
  std::unordered_map<std::string, Value> tags;
  std::vector<LogRecordData> logs;
};

class Recorder {
 public:
   virtual ~Recorder() = default;

   virtual void RecordSpan(SpanData&& span_data) noexcept = 0;
};

class InMemoryRecorder : public Recorder {
  void RecordSpan(SpanData&& span_data) noexcept override {}
};

std::vector<SpanData> WriteSpans(std::ostream& ostream);
void ReadSpans(const std::vector<SpanData>& spans, std::istream& istream);

struct MockTracerOptions {
  std::unique_ptr<Recorder> recorder;
};

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

  expected<void> Inject(const SpanContext& /*sc*/,
                        std::ostream& /*writer*/) const override {
    return {};
  }

  expected<void> Inject(const SpanContext& /*sc*/,
                        const TextMapWriter& /*writer*/) const override {
    return {};
  }

  expected<void> Inject(const SpanContext& /*sc*/,
                        const HTTPHeadersWriter& /*writer*/) const override {
    return {};
  }

  expected<std::unique_ptr<SpanContext>> Extract(
      std::istream& /*reader*/) const override {
    return std::unique_ptr<SpanContext>(nullptr);
  }

  expected<std::unique_ptr<SpanContext>> Extract(
      const TextMapReader& /*reader*/) const override {
    return std::unique_ptr<SpanContext>(nullptr);
  }

  expected<std::unique_ptr<SpanContext>> Extract(
      const HTTPHeadersReader& /*reader*/) const override {
    return std::unique_ptr<SpanContext>(nullptr);
  }

 private:
  std::unique_ptr<Recorder> recorder_;
  std::mutex mutex_;
  std::vector<SpanData> spans_;
};
} // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
#endif  // OPENTRACING_MOCKTRACER_H
