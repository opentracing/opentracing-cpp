#ifndef OPENTRACING_MOCKTRACER_IN_MEMORY_RECORDER_H
#define OPENTRACING_MOCKTRACER_IN_MEMORY_RECORDER_H

#include <opentracing/mocktracer/recorder.h>
#include <opentracing/mocktracer/symbols.h>
#include <mutex>
#include <vector>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
// InMemoryRecorder stores finished spans and provides accessors to them.
class OPENTRACING_MOCK_TRACER_API InMemoryRecorder : public Recorder {
 public:
  void RecordSpan(SpanData&& span_data) noexcept override;

  // Returns a vector of all finished spans.
  std::vector<SpanData> spans() const;

  // Returns the number of finished spans.
  size_t size() const;

  // Returns the last finished span. Throws if no spans have been finished.
  SpanData top() const;

 private:
  mutable std::mutex mutex_;
  std::vector<SpanData> spans_;
};
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_IN_MEMORY_RECORDER_H
