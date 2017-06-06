#ifndef OPENTRACING_TRACER_H
#define OPENTRACING_TRACER_H

#include <opentracing/util.h>
#include <opentracing/span.h>
#include <opentracing/span_context.h>
#include <memory>
#include <initializer_list>

namespace opentracing {
enum class SpanReferenceType {
  // ChildOfRef refers to a parent Span that caused *and* somehow depends
  // upon the new child Span. Often (but not always), the parent Span cannot
  // finish unitl the child Span does.
  //
  // An timing diagram for a ChildOfRef that's blocked on the new Span:
  //
  //     [-Parent Span---------]
  //          [-Child Span----]
  //
  // See http://opentracing.io/spec/
  //
  // See opentracing.ChildOf()
  ChildOfRef = 1,

  // FollowsFromRef refers to a parent Span that does not depend in any way
  // on the result of the new child Span. For instance, one might use
  // FollowsFromRefs to describe pipeline stages separated by queues,
  // or a fire-and-forget cache insert at the tail end of a web request.
  //
  // A FollowsFromRef Span is part of the same logical trace as the new Span:
  // i.e., the new Span is somehow caused by the work of its FollowsFromRef.
  //
  // All of the following could be valid timing diagrams for children that
  // "FollowFrom" a parent.
  //
  //     [-Parent Span-]  [-Child Span-]
  //
  //
  //     [-Parent Span--]
  //      [-Child Span-]
  //
  //
  //     [-Parent Span-]
  //                 [-Child Span-]
  //
  // See http://opentracing.io/spec/
  //
  // See opentracing.FollowsFrom()
  FollowsFromRef = 2
};

struct StartSpanOptions {
  SystemTime start_system_timestamp;
  SteadyTime start_steady_timestamp;
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
};
} // namespace opentracing

#endif // OPENTRACING_TRACER_H
