#ifndef OPENTRACING_TRACER_H
#define OPENTRACING_TRACER_H

#include <opentracing/preprocessor.h>
#include <opentracing/propagation.h>
#include <opentracing/span.h>
#include <opentracing/stringref.h>
#include <opentracing/util.h>
#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

namespace opentracing {
inline namespace OPENTRACING_VERSION_NAMESPACE {
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

  virtual void Apply(StartSpanOptions& options) const = 0;

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
  std::unique_ptr<Span> StartSpan(
      StringRef operation_name,
      std::initializer_list<option_wrapper<StartSpanOption>> option_list = {})
      const {
    StartSpanOptions options;
    options.start_system_timestamp = SystemClock::now();
    options.start_steady_timestamp = SteadyClock::now();
    for (const auto& option : option_list) option.get().Apply(options);
    return StartSpanWithOptions(operation_name, options);
  }

  virtual std::unique_ptr<Span> StartSpanWithOptions(
      StringRef operation_name, const StartSpanOptions& options) const = 0;

  // Inject() takes the `sc` SpanContext instance and injects it for
  // propagation within `carrier`. The actual type of `carrier` depends on
  // the value of `format`.
  //
  // OpenTracing defines a common set of `format` values (see BuiltinFormat),
  // and each has an expected carrier type.
  virtual Expected<void, std::string> Inject(
      const SpanContext& sc, CarrierFormat format,
      const CarrierWriter& writer) const = 0;

  // Extract() returns a SpanContext instance given `format` and `carrier`.
  //
  // OpenTracing defines a common set of `format` values (see BuiltinFormat),
  // and each has an expected carrier type.
  //
  // Returns a `SpanContext` that is `non-null` on success.
  virtual std::unique_ptr<SpanContext> Extract(
      CarrierFormat format, const CarrierReader& reader) const = 0;

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
  StartTimestamp(SystemTime system_when, SteadyTime steady_when)
      : system_when_(system_when), steady_when_(steady_when) {}

  template <class Rep, class Period>
  explicit StartTimestamp(
      const std::chrono::duration<Rep, Period>& time_since_epoch)
      : system_when_(time_since_epoch), steady_when_(time_since_epoch) {}

  StartTimestamp(const StartTimestamp& other)
      : StartSpanOption(),
        system_when_(other.system_when_),
        steady_when_(other.steady_when_) {}

  void Apply(StartSpanOptions& options) const override {
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
  SpanReference(SpanReferenceType type, const SpanContext& referenced)
      : type_(type), referenced_(referenced) {}

  SpanReference(const SpanReference& other)
      : StartSpanOption(), type_(other.type_), referenced_(other.referenced_) {}

  void Apply(StartSpanOptions& options) const override {
    options.references.emplace_back(type_, &referenced_);
  }

 private:
  SpanReferenceType type_;
  const SpanContext& referenced_;
};

// ChildOf returns a StartSpanOption pointing to a dependent parent span.
//
// See ChildOfRef, SpanReference
inline SpanReference ChildOf(const SpanContext& span_context) {
  return {SpanReferenceType::ChildOfRef, span_context};
}

// FollowsFrom returns a StartSpanOption pointing to a parent Span that caused
// the child Span but does not directly depend on its result in any way.
//
// See FollowsFromRef, SpanReference
inline SpanReference FollowsFrom(const SpanContext& span_context) {
  return {SpanReferenceType::FollowsFromRef, span_context};
}

// SetTag may be passed as a StartSpanOption to add a tag to new spans,
// for example:
//
// tracer.StartSpan("opName", SetTag{"Key", value})
class SetTag : public StartSpanOption {
 public:
  SetTag(StringRef key, const Value& value) : key_(key), value_(value) {}

  SetTag(const SetTag& other)
      : StartSpanOption(), key_(other.key_), value_(other.value_) {}

  void Apply(StartSpanOptions& options) const override {
    options.tags.emplace_back(key_, value_);
  }

 private:
  StringRef key_;
  const Value& value_;
};
}  // namespace OPENTRACING_VERSION_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_TRACER_H
