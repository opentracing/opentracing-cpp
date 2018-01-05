#ifndef OPENTRACING_MOCKTRACER_IN_MEMORY_RECORDER_H
#define OPENTRACING_MOCKTRACER_IN_MEMORY_RECORDER_H

#include <opentracing/mocktracer/recorder.h>
#include <mutex>
#include <vector>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
class InMemoryRecorder : public Recorder {
 public:
  void RecordSpan(SpanData&& span_data) noexcept override;

  std::vector<SpanData> spans() const;

  size_t size() const;

  SpanData top() const;
 private:
  mutable std::mutex mutex_;
  std::vector<SpanData> spans_;
};
} // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_IN_MEMORY_RECORDER_H
