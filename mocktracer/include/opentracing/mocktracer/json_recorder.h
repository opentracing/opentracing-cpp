#ifndef OPENTRACING_MOCKTRACER_JSON_RECORDER_H
#define OPENTRACING_MOCKTRACER_JSON_RECORDER_H

#include <opentracing/mocktracer/recorder.h>
#include <opentracing/mocktracer/symbols.h>
#include <iosfwd>
#include <memory>
#include <mutex>
#include <vector>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
// JsonRecorder serializes finished spans to a provided std::ostream in a JSON
// format.
//
// See also FromJson.
class OPENTRACING_MOCK_TRACER_API JsonRecorder : public Recorder {
 public:
  explicit JsonRecorder(std::unique_ptr<std::ostream>&& out);

  void RecordSpan(SpanData&& span_data) noexcept override;

  void Close() noexcept override;

 private:
  std::mutex mutex_;
  std::unique_ptr<std::ostream> out_;
  std::vector<SpanData> spans_;
};
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_JSON_RECORDER_H
