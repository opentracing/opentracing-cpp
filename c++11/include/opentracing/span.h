#ifndef OPENTRACING_SPAN_H
#define OPENTRACING_SPAN_H

#include <chrono>
#include <functional>
#include <string>

#include <opentracing/util.h>
#include <opentracing/value.h>

namespace opentracing {
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
};

// FinishOptions allows Span.Finish callers to override the finish
// timestamp.
struct FinishSpanOptions {
  SteadyTime finish_steady_timestamp;
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
  virtual ~Span() = default;

  // Sets the end timestamp and finalizes Span state.
  //
  // With the exception of calls to context() (which are always allowed),
  // Finish() must be the last call made to any span instance, and to do
  // otherwise leads to undefined behavior.
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
  // SetOperationName may be called prior to Finish.
  virtual void SetOperationName(const std::string& name) = 0;

  // Adds a tag to the span.
  //
  // If there is a pre-existing tag set for `key`, it is overwritten.
  //
  // Tag values can be numeric types, strings, or bools. The behavior of
  // other tag value types is undefined at the OpenTracing level. If a
  // tracing system does not know how to handle a particular value type, it
  // may ignore the tag, but shall not panic.
  //
  // SetTag may be called prior to Finish.
  virtual void SetTag(const std::string& key, const Value& value) = 0;

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
  // SetBaggageItem may be be called prior to Finish.
  virtual void SetBaggageItem(const std::string& restricted_key,
                              const std::string& value) = 0;

  // Gets the value for a baggage item given its key. Returns the empty string
  // if the value isn't found in this Span.
  virtual std::string BaggageItem(const std::string& restricted_key) const = 0;

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

  template <class Rep, class Period>
  explicit FinishTimestamp(
      const std::chrono::duration<Rep, Period>& time_since_epoch)
      : steady_when_(time_since_epoch) {}

  FinishTimestamp(const FinishTimestamp& other) noexcept
      : FinishSpanOption(), steady_when_(other.steady_when_) {}

  void Apply(FinishSpanOptions& options) const noexcept override {
    options.finish_steady_timestamp = steady_when_;
  }

 private:
  SteadyTime steady_when_;
};
}  // namespace opentracing

#endif  // OPENTRACING_SPAN_H
