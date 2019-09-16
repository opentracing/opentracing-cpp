#ifndef OPENTRACING_SPAN_H
#define OPENTRACING_SPAN_H

#include <opentracing/string_view.h>
#include <opentracing/util.h>
#include <opentracing/value.h>
#include <opentracing/version.h>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
class Tracer;

// SpanContext represents Span state that must propagate to descendant Spans and
// across process boundaries (e.g., a <trace_id, span_id, sampled> tuple).
class SpanContext {
 public:
  virtual ~SpanContext() = default;

  // ForeachBaggageItem calls a function for each baggage item in the
  // context.  If the function returns false, it will not be called
  // again and ForeachBaggageItem will return.
  virtual void ForeachBaggageItem(
      std::function<bool(const std::string& key, const std::string& value)> f)
      const = 0;

  // Clone creates a copy of SpanContext.
  //
  // Returns nullptr on failure.
  virtual std::unique_ptr<SpanContext> Clone() const noexcept = 0;

  // Return the ID of the trace.
  //
  // Should be globally unique. Every span in a trace shares this ID.
  //
  // An empty string will be returned if the tracer does not support this
  // functionality or an error occurs (this is the case for no-op traces, for
  // example).
  virtual std::string ToTraceID() const noexcept { return {}; }

  // Return the ID of the associated Span.
  //
  // Should be unique within a trace. Each span within a trace contains a
  // different ID.
  //
  // An empty string will be returned if the tracer does not support this
  // functionality or an error occurs (this is the case for no-op traces, for
  // example).
  virtual std::string ToSpanID() const noexcept { return {}; }
};

struct LogRecord {
  using Field = std::pair<std::string, Value>;

  SystemTime timestamp;
  std::vector<Field> fields;
};

inline bool operator==(const LogRecord& lhs, const LogRecord& rhs) {
  return lhs.timestamp == rhs.timestamp && lhs.fields == rhs.fields;
}

inline bool operator!=(const LogRecord& lhs, const LogRecord& rhs) {
  return !(lhs == rhs);
}

// FinishOptions allows Span.Finish callers to override the finish
// timestamp.
struct FinishSpanOptions {
  SteadyTime finish_steady_timestamp;

  // log_records allows the caller to specify the contents of many Log() calls
  // with a single vector. May be empty.
  //
  // None of the LogRecord.timestamp values may be SystemTime() (i.e., they must
  // be set explicitly). Also, they must be >= the Span's start system timestamp
  // and <= the finish_steady_timestamp converted to system timestamp
  // (or SystemTime::now() if finish_steady_timestamp is default-constructed).
  // Otherwise the behavior of FinishWithOptions() is unspecified.
  std::vector<LogRecord> log_records;
};

// FinishSpanOption instances (zero or more) may be passed to Span.Finish.
class FinishSpanOption {
 public:
  FinishSpanOption(const FinishSpanOption&) = delete;

  virtual ~FinishSpanOption() = default;

  virtual void Apply(FinishSpanOptions& options) const noexcept = 0;

 protected:
  FinishSpanOption() = default;
};

// Span represents an active, un-finished span in the OpenTracing system.
//
// Spans are created by the Tracer interface.
class Span {
 public:
  // If Finish has not already been called for the Span, it's destructor must
  // do so.
  virtual ~Span() = default;

  // Sets the end timestamp and finalizes Span state.
  //
  // If Finish is called a second time, it is guaranteed to do nothing.
  void Finish(std::initializer_list<option_wrapper<FinishSpanOption>>
                  option_list = {}) noexcept {
    FinishSpanOptions options;
    options.finish_steady_timestamp = SteadyClock::now();
    for (const auto& option : option_list) option.get().Apply(options);
    FinishWithOptions(options);
  }

  virtual void FinishWithOptions(
      const FinishSpanOptions& finish_span_options) noexcept = 0;

  // Sets or changes the operation name.
  //
  // If SetOperationName is called after Finish it leaves the Span in a valid
  // state, but its behavior is unspecified.
  virtual void SetOperationName(string_view name) noexcept = 0;

  // Adds a tag to the span.
  //
  // If there is a pre-existing tag set for `key`, it is overwritten.
  //
  // Tag values can be numeric types, strings, or bools. The behavior of
  // other tag value types is undefined at the OpenTracing level. If a
  // tracing system does not know how to handle a particular value type, it
  // may ignore the tag, but shall not panic.
  //
  // If SetTag is called after Finish it leaves the Span in a valid state, but
  // its behavior is unspecified.
  virtual void SetTag(string_view key, const Value& value) noexcept = 0;

  // SetBaggageItem sets a key:value pair on this Span and its SpanContext
  // that also propagates to descendants of this Span.
  //
  // SetBaggageItem() enables powerful functionality given a full-stack
  // opentracing integration (e.g., arbitrary application data from a mobile
  // app can make it, transparently, all the way into the depths of a storage
  // system), and with it some powerful costs: use this feature with care.
  //
  // IMPORTANT NOTE #1: SetBaggageItem() will only propagate baggage items to
  // *future* causal descendants of the associated Span.
  //
  // IMPORTANT NOTE #2: Use this thoughtfully and with care. Every key and
  // value is copied into every local *and remote* child of the associated
  // Span, and that can add up to a lot of network and cpu overhead.
  //
  // If SetBaggageItem is called after Finish it leaves the Span in a valid
  // state, but its behavior is unspecified.
  virtual void SetBaggageItem(string_view restricted_key,
                              string_view value) noexcept = 0;

  // Gets the value for a baggage item given its key. Returns the empty string
  // if the value isn't found in this Span.
  virtual std::string BaggageItem(string_view restricted_key) const
      noexcept = 0;

  // Log is an efficient and type-checked way to record key:value logging data
  // about a Span. Here's an example:
  //
  //    span.Log({
  //        {"event", "soft error"},
  //        {"type", "cache timeout"},
  //        {"waited.millis", 1500}});
  virtual void Log(
      std::initializer_list<std::pair<string_view, Value>> fields) noexcept = 0;

  virtual void Log(
      SystemTime timestamp,
      std::initializer_list<std::pair<string_view, Value>> fields) noexcept = 0;

  virtual void Log(
      SystemTime timestamp,
      const std::vector<std::pair<string_view, Value>>& fields) noexcept = 0;

  // context() yields the SpanContext for this Span. Note that the return
  // value of context() is still valid after a call to Span.Finish(), as is
  // a call to Span.context() after a call to Span.Finish().
  virtual const SpanContext& context() const noexcept = 0;

  // Provides access to the Tracer that created this Span.
  virtual const Tracer& tracer() const noexcept = 0;
};

// FinishTimestamp is a FinishSpanOption that sets an explicit finish timestamp
// for a Span.
class FinishTimestamp : public FinishSpanOption {
 public:
  explicit FinishTimestamp(SteadyTime steady_when) noexcept
      : steady_when_(steady_when) {}

  // Construct a timestamp using a duration from the epoch of std::time_t.
  // From the documentation on std::time_t's epoch:
  //     Although not defined, this is almost always an integral value holding
  //     the number of seconds (not counting leap seconds) since 00:00, Jan 1
  //     1970 UTC, corresponding to POSIX time
  // See http://en.cppreference.com/w/cpp/chrono/c/time_t
  template <class Rep, class Period>
  explicit FinishTimestamp(
      const std::chrono::duration<Rep, Period>& time_since_epoch) noexcept
      : steady_when_(convert_time_point<SteadyClock>(
            SystemClock::from_time_t(std::time_t(0)) +
            std::chrono::duration_cast<SystemClock::duration>(
                time_since_epoch))) {}

  FinishTimestamp(const FinishTimestamp& other) noexcept
      : FinishSpanOption(), steady_when_(other.steady_when_) {}

  void Apply(FinishSpanOptions& options) const noexcept override {
    options.finish_steady_timestamp = steady_when_;
  }

 private:
  SteadyTime steady_when_;
};
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_SPAN_H
