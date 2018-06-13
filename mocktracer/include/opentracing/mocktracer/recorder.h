#ifndef OPENTRACING_MOCKTRACER_RECORDER_H
#define OPENTRACING_MOCKTRACER_RECORDER_H

#include <opentracing/mocktracer/symbols.h>
#include <opentracing/tracer.h>

#include <cstdint>
#include <iosfwd>
#include <map>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
struct SpanContextData {
  uint64_t trace_id;
  uint64_t span_id;
  std::map<std::string, std::string> baggage;
};

inline bool operator==(const SpanContextData& lhs, const SpanContextData& rhs) {
  return lhs.trace_id == rhs.trace_id && lhs.span_id == rhs.span_id &&
         lhs.baggage == rhs.baggage;
}

inline bool operator!=(const SpanContextData& lhs, const SpanContextData& rhs) {
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out,
                         const SpanContextData& span_context_data);

struct SpanReferenceData {
  SpanReferenceType reference_type;
  uint64_t trace_id;
  uint64_t span_id;
};

inline bool operator==(const SpanReferenceData& lhs,
                       const SpanReferenceData& rhs) {
  return lhs.reference_type == rhs.reference_type &&
         lhs.trace_id == rhs.trace_id && lhs.span_id == rhs.span_id;
}

inline bool operator!=(const SpanReferenceData& lhs,
                       const SpanReferenceData& rhs) {
  return !(lhs == rhs);
}

struct SpanData {
  SpanContextData span_context;
  std::vector<SpanReferenceData> references;
  std::string operation_name;
  SystemTime start_timestamp;
  SteadyClock::duration duration;
  std::map<std::string, Value> tags;
  std::vector<LogRecord> logs;
};

inline bool operator==(const SpanData& lhs, const SpanData& rhs) {
  return lhs.span_context == rhs.span_context &&
         lhs.references == rhs.references &&
         lhs.operation_name == rhs.operation_name &&
         lhs.start_timestamp == rhs.start_timestamp &&
         lhs.duration == rhs.duration && lhs.tags == rhs.tags &&
         lhs.logs == rhs.logs;
}

inline bool operator!=(const SpanData& lhs, const SpanData& rhs) {
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const SpanData& span_data);

class OPENTRACING_MOCK_TRACER_API Recorder {
 public:
  virtual ~Recorder() = default;

  virtual void RecordSpan(SpanData&& span_data) noexcept = 0;

  virtual void Close() noexcept {}
};

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_RECORDER_H
