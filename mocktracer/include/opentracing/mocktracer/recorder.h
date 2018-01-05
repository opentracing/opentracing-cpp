#ifndef OPENTRACING_MOCKTRACER_RECORDER_H
#define OPENTRACING_MOCKTRACER_RECORDER_H

#include <opentracing/tracer.h>
#include <cstdint>

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

struct SpanReferenceData {
  SpanReferenceType reference_type;
  uint64_t trace_id;
  uint64_t span_id;
};

struct SpanData {
  SpanContextData span_context;
  std::vector<SpanReferenceData> references;
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
} // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_RECORDER_H
