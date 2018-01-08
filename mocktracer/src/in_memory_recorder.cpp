#include <opentracing/mocktracer/in_memory_recorder.h>
#include <stdexcept>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
void InMemoryRecorder::RecordSpan(SpanData&& span_data) noexcept try {
  std::lock_guard<std::mutex> lock_guard{mutex_};
  spans_.emplace_back(std::move(span_data));
} catch (const std::exception&) {
  // Drop span.
}

std::vector<SpanData> InMemoryRecorder::spans() const {
  std::lock_guard<std::mutex> lock_guard{mutex_};
  return spans_;
}

size_t InMemoryRecorder::size() const {
  std::lock_guard<std::mutex> lock_guard{mutex_};
  return spans_.size();
}

SpanData InMemoryRecorder::top() const {
  std::lock_guard<std::mutex> lock_guard{mutex_};
  if (spans_.empty()) {
    throw std::runtime_error{"no spans"};
  }
  return spans_.back();
}
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
