#include <opentracing/mocktracer/json.h>
#include <opentracing/mocktracer/json_recorder.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
JsonRecorder::JsonRecorder(std::unique_ptr<std::ostream>&& out)
    : out_{std::move(out)} {}

void JsonRecorder::RecordSpan(SpanData&& span_data) noexcept try {
  std::lock_guard<std::mutex> lock_guard{mutex_};
  spans_.emplace_back(std::move(span_data));
} catch (const std::exception&) {
  // Drop span.
}

void JsonRecorder::Close() noexcept try {
  if (out_ == nullptr) {
    return;
  }
  std::lock_guard<std::mutex> lock_guard{mutex_};
  ToJson(*out_, spans_);
  out_->flush();
  spans_.clear();
} catch (const std::exception&) {
  // Ignore errors.
}
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
