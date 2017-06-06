#ifndef OPENTRACING_SPAN_H
#define OPENTRACING_SPAN_H

#include <string>
#include <chrono>

#include <opentracing/value.h>
#include <opentracing/util.h>

namespace opentracing {
class SpanContext;

struct FinishSpanOptions {
  SteadyTime finish_steady_timestamp;
};

class FinishSpanOption {
 public:
   FinishSpanOption(const FinishSpanOption&) = delete;

   virtual ~FinishSpanOption() = default;

   virtual void Apply(FinishSpanOptions& options) const = 0;
 protected:
   FinishSpanOption() = default;
};

class FinishTimestamp : public FinishSpanOption {
 public:
  explicit FinishTimestamp(SteadyTime steady_when)
      : steady_when_(steady_when) {}

  void Apply(FinishSpanOptions& options) const override {
    options.finish_steady_timestamp = steady_when_;
  }

 private:
  SteadyTime steady_when_;
};

class Span {
 public:
   virtual ~Span() = default;

   void Finish(
       std::initializer_list<option_wrapper<FinishSpanOption>> option_list) {
     FinishSpanOptions options;
     for (const auto& option : option_list)
       option.get().Apply(options);
     Finish(options);
   }

   virtual void Finish(const FinishSpanOptions& finish_span_options) = 0;

   virtual void SetOperationName(const std::string& name) = 0;

   virtual void SetTag(const std::string& key, const Value& value) = 0;

   virtual void SetBaggageItem(const std::string& restricted_key,
                               const std::string& value) = 0;

   virtual std::string BaggageItem(const std::string& restricted_key) const = 0;

   virtual const SpanContext& context() const = 0;
};
} // namespace opentracing

#endif // OPENTRACING_SPAN_H
