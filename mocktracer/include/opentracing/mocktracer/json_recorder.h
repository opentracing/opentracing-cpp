#ifndef OPENTRACING_MOCKTRACER_JSON_RECORDER_H
#define OPENTRACING_MOCKTRACER_JSON_RECORDER_H

#include <opentracing/mocktracer/recorder.h>
#include <mutex>
#include <vector>
#include <iosfwd>
#include <memory>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
class JsonRecorder : public Recorder {
 public:
  explicit JsonRecorder(std::unique_ptr<std::ostream>&& out);

  void RecordSpan(SpanData&& span_data) noexcept override;

  void Flush() noexcept override;
 private:
  std::mutex mutex_;
  std::unique_ptr<std::ostream> out_;
  std::vector<SpanData> spans_;
};
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_JSON_RECORDER_H
