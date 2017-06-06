#ifndef OPENTRACING_TRACER_H
#define OPENTRACING_TRACER_H

#include <opentracing/util.h>
#include <opentracing/span.h>
#include <opentracing/span_context.h>
#include <opentracing/propagation.h>
#include <memory>
#include <vector>
#include <utility>
#include <initializer_list>

namespace opentracing {
struct StartSpanOptions {
  SystemTime start_system_timestamp;
  SteadyTime start_steady_timestamp;
  std::vector<std::pair<SpanReferenceType, const SpanContext*>> references;
  std::vector<std::pair<std::string, Value>> tags;
};

class StartSpanOption {
 public:
   StartSpanOption(const StartSpanOption&) = delete;

   virtual ~StartSpanOption() = default;

   virtual void Apply(StartSpanOptions& options) const = 0;
 protected:
   StartSpanOption() = default;
};

class StartTimestamp : public StartSpanOption {
 public:
   StartTimestamp(SystemTime system_when, SteadyTime steady_when)
     : system_when_(system_when), steady_when_(steady_when)
   {}

   void Apply(StartSpanOptions& options) const override {
     options.start_system_timestamp = system_when_;
     options.start_steady_timestamp = steady_when_;
   }
 private:
   SystemTime system_when_;
   SteadyTime steady_when_;
};

class SpanReference : public StartSpanOption {
 public:
   SpanReference(SpanReferenceType type, const SpanContext* referenced)
     : type_(type), referenced_(referenced)
   {}

   void Apply(StartSpanOptions& options) const override {
     options.references.emplace_back(type_, referenced_);
   }
 private:
   SpanReferenceType type_;
   const SpanContext* referenced_;
};

inline SpanReference ChildOf(const SpanContext& span_context) {
  return {SpanReferenceType::ChildOfRef, &span_context};
}

inline SpanReference FollowsFrom(const SpanContext& span_context) {
  return {SpanReferenceType::FollowsFromRef, &span_context}; 
}

class SetTag : public StartSpanOption {
 public:
   SetTag(const std::string& key, const Value& value)
     : key_(key), value_(value) {}
   SetTag(const SetTag& other) : key_(other.key_), value_(other.value_) {}

   void Apply(StartSpanOptions& options) const override {
     options.tags.emplace_back(key_, value_);
   }
 private:
     const std::string& key_;
     const Value& value_;
};

class Tracer {
 public:
   virtual ~Tracer() = default;

   std::unique_ptr<Span> StartSpan(const std::string& operation_name,
       std::initializer_list<option_wrapper<StartSpanOption>> option_list) {
     StartSpanOptions options;
     for (const auto& option : option_list)
       option.get().Apply(options);
     return StartSpan(operation_name, options);
   }

   virtual std::unique_ptr<Span> StartSpan(
       const std::string& operation_name,
       const StartSpanOptions& options = {}) = 0;

  // Inject() takes the `sc` SpanContext instance and injects it for
  // propagation within `carrier`. The actual type of `carrier` depends on
  // the value of `format`.
  //
  // OpenTracing defines a common set of `format` values (see BuiltinFormat),
  // and each has an expected carrier type.
  //
  // Returns true on success.
   virtual bool Inject(SpanContext sc, CarrierFormat format,
                       const CarrierWriter& writer) = 0;

  // Extract() returns a SpanContext instance given `format` and `carrier`.
  //
  // OpenTracing defines a common set of `format` values (see BuiltinFormat),
  // and each has an expected carrier type.
  //
  // Returns a `SpanContext` that is `valid()` on success.
   virtual std::unique_ptr<SpanContext> Extract(
       CarrierFormat format, const CarrierReader& reader) = 0;
};
} // namespace opentracing

#endif // OPENTRACING_TRACER_H
