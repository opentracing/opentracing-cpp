#ifndef OPENTRACING_TRACER_H
#define OPENTRACING_TRACER_H

#include <opentracing/propagation.h>
#include <opentracing/span.h>
#include <opentracing/stringref.h>
#include <opentracing/util.h>
#include <opentracing/version.h>
#include <chrono>
#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
// StartSpanOptions allows Tracer.StartSpan() callers  a mechanism to override
// the start timestamp, specify Span References, and make a single Tag or
// multiple Tags available at Span start time.
//
// StartSpan() callers should look at the StartSpanOption interface and
// implementations available in this library.
struct StartSpanOptions {
  SystemTime start_system_timestamp;
  SteadyTime start_steady_timestamp;
  std::vector<std::pair<SpanReferenceType, const SpanContext*>> references;
  std::vector<std::pair<StringRef, Value>> tags;
};

// StartSpanOption instances (zero or more) may be passed to Tracer.StartSpan.
class StartSpanOption {
 public:
  StartSpanOption(const StartSpanOption&) = delete;

  virtual ~StartSpanOption() = default;

  virtual void Apply(StartSpanOptions& options) const noexcept = 0;

 protected:
  StartSpanOption() = default;
};

// Tracer is a simple, thin interface for Span creation and SpanContext
// propagation.
class Tracer {
 public:
  virtual ~Tracer() = default;

  // Create, start, and return a new Span with the given `operationName` and
  // incorporate the given StartSpanOption `option_list`.
  //
  // A Span with no SpanReference options (e.g., opentracing::ChildOf() or
  // opentracing::FollowsFrom()) becomes the root of its own trace.
  //
  // Examples:
  //
  //     opentracing::Tracer& tracer = ...
  //
  //     // The root-span case:
  //     auto span = tracer.StartSpan("GetFeed")
  //
  //     // The vanilla child span case:
  //     auto span = tracer.StartSpan(
  //         "GetFeed",
  //         {opentracing::ChildOf(parentSpan.context())})
  //
  //     // All the bells and whistles:
  //     auto span = tracer.StartSpan(
  //         "GetFeed",
  //         {opentracing::ChildOf(parentSpan.context()),
  //         opentracing::Tag{"user_agent", loggedReq.UserAgent},
  //         opentracing::StartTimestamp(loggedReq.timestamp())})
  //
  // If StartSpan is called after Close it leaves the Tracer in a valid
  // state, but its behavior is unspecified.
  std::unique_ptr<Span> StartSpan(
      StringRef operation_name,
      std::initializer_list<option_wrapper<StartSpanOption>> option_list = {})
      const noexcept {
    StartSpanOptions options;
    options.start_system_timestamp = SystemClock::now();
    options.start_steady_timestamp = SteadyClock::now();
    for (const auto& option : option_list) option.get().Apply(options);
    return StartSpanWithOptions(operation_name, options);
  }

  virtual std::unique_ptr<Span> StartSpanWithOptions(
      StringRef operation_name, const StartSpanOptions& options) const
      noexcept = 0;

  // Inject() takes the `sc` SpanContext instance and injects it for
  // propagation within `carrier`.
  //
  // OpenTracing defines a common set of `carrier` interfaces.
  //
  // Throws only if `writer` does.
  virtual Expected<void> Inject(const SpanContext& sc,
                                const TextMapWriter& writer) const = 0;

  virtual Expected<void> Inject(const SpanContext& sc,
                                const HTTPHeadersWriter& writer) const = 0;

  virtual Expected<void> Inject(const SpanContext& sc,
                                const CustomCarrierWriter& writer) const {
    return writer.Inject(*this, sc);
  }

  // Extract() returns a SpanContext instance given `carrier`.
  //
  // OpenTracing defines a common set of `carrier` interfaces.
  //
  // Returns a `SpanContext` that is `non-null` on success or nullptr if
  // no span is found; otherwise an std::error_code from
  // propagation_error_category().
  //
  // Throws only if `reader` does.
  virtual Expected<std::unique_ptr<SpanContext>> Extract(
      const TextMapReader& reader) const = 0;

  virtual Expected<std::unique_ptr<SpanContext>> Extract(
      const HTTPHeadersReader& reader) const = 0;

  virtual Expected<std::unique_ptr<SpanContext>> Extract(
      const CustomCarrierReader& reader) const {
    return reader.Extract(*this);
  }

  // Close is called when a tracer is finished processing spans. It is not
  // required to be called and its effect is unspecified. For example, an
  // implementation might use this function to flush buffered spans to its
  // recording system and failing to call it could result in some spans being
  // dropped.
  virtual void Close() noexcept {}

  // GlobalTracer returns the global tracer.
  static std::shared_ptr<Tracer> Global() noexcept;

  // InitGlobalTracer sets the global tracer pointer, returns the
  // former global tracer value.
  static std::shared_ptr<Tracer> InitGlobal(
      std::shared_ptr<Tracer> tracer) noexcept;
};

// StartTimestamp is a StartSpanOption that sets an explicit start timestamp for
// the new Span.
class StartTimestamp : public StartSpanOption {
 public:
  StartTimestamp(SystemTime system_when, SteadyTime steady_when) noexcept
      : system_when_(system_when), steady_when_(steady_when) {}

  StartTimestamp(SystemTime system_when) noexcept
      : system_when_(system_when),
        steady_when_(convert_time_point<SteadyClock>(system_when_)) {}

  // Construct a timestamp using a duration from the epoch of std::time_t.
  // From the documentation on std::time_t's epoch:
  //     Although not defined, this is almost always an integral value holding
  //     the number of seconds (not counting leap seconds) since 00:00, Jan 1
  //     1970 UTC, corresponding to POSIX time
  // See http://en.cppreference.com/w/cpp/chrono/c/time_t
  template <class Rep, class Period>
  explicit StartTimestamp(
      const std::chrono::duration<Rep, Period>& time_since_epoch) noexcept
      : StartTimestamp(SystemClock::from_time_t(std::time_t(0)) +
                       std::chrono::duration_cast<SystemClock::duration>(
                           time_since_epoch)) {}

  StartTimestamp(const StartTimestamp& other) noexcept
      : StartSpanOption(),
        system_when_(other.system_when_),
        steady_when_(other.steady_when_) {}

  void Apply(StartSpanOptions& options) const noexcept override {
    options.start_system_timestamp = system_when_;
    options.start_steady_timestamp = steady_when_;
  }

 private:
  SystemTime system_when_;
  SteadyTime steady_when_;
};

// SpanReference is a StartSpanOption that pairs a SpanReferenceType and a
// referenced SpanContext. See the SpanReferenceType documentation for
// supported relationships.
class SpanReference : public StartSpanOption {
 public:
  SpanReference(SpanReferenceType type, const SpanContext* referenced) noexcept
      : type_(type), referenced_(referenced) {}

  SpanReference(const SpanReference& other) noexcept
      : StartSpanOption(), type_(other.type_), referenced_(other.referenced_) {}

  void Apply(StartSpanOptions& options) const noexcept override try {
    if (referenced_) options.references.emplace_back(type_, referenced_);
  } catch (const std::bad_alloc&) {
    // Ignore reference if memory can't be allocated for it.
  }

 private:
  SpanReferenceType type_;
  const SpanContext* referenced_;
};

// ChildOf returns a StartSpanOption pointing to a dependent parent span.
//
// See ChildOfRef, SpanReference
inline SpanReference ChildOf(const SpanContext* span_context) noexcept {
  return {SpanReferenceType::ChildOfRef, span_context};
}

// FollowsFrom returns a StartSpanOption pointing to a parent Span that caused
// the child Span but does not directly depend on its result in any way.
//
// See FollowsFromRef, SpanReference
inline SpanReference FollowsFrom(const SpanContext* span_context) noexcept {
  return {SpanReferenceType::FollowsFromRef, span_context};
}

// SetTag may be passed as a StartSpanOption to add a tag to new spans,
// for example:
//
// tracer.StartSpan("opName", SetTag{"Key", value})
class SetTag : public StartSpanOption {
 public:
  SetTag(StringRef key, const Value& value) noexcept
      : key_(key), value_(value) {}

  SetTag(const SetTag& other) noexcept
      : StartSpanOption(), key_(other.key_), value_(other.value_) {}

  void Apply(StartSpanOptions& options) const noexcept override try {
    options.tags.emplace_back(key_, value_);
  } catch (const std::bad_alloc&) {
    // Ignore tag if memory can't be allocated for it.
  }

 private:
  StringRef key_;
  const Value& value_;
};
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_TRACER_H
