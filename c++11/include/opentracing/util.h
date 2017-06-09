#ifndef OPENTRACING_UTIL_H
#define OPENTRACING_UTIL_H

#include <opentracing/preprocessor.h>
#include <chrono>
#include <opentracing/martinmoene_expected/expected.hpp>

namespace opentracing {
inline namespace OPENTRACING_VERSION_NAMESPACE {
using SystemClock = std::chrono::system_clock;
using SteadyClock = std::chrono::steady_clock;
using SystemTime = SystemClock::time_point;
using SteadyTime = SteadyClock::time_point;

template <class T, class E>
using Expected = nonstd::expected<T, E>;
using nonstd::make_unexpected;

// This is unsafe to do.
//
// This is like an unsafe std::reference_wrapper<> that allows taking
// references to temporaries.  It must only be used for temporary
// SpanStartOption and SpanFinishOption objects.
template <typename T>
class option_wrapper {
 public:
  option_wrapper(const T &opt) : ptr_(&opt) {}

  // This will dangle unless it is only used for short-lived initializer lists.
  const T &get() const { return *ptr_; }

 private:
  const T *ptr_;
};
}  // namespace OPENTRACING_VERSION_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_UTIL_H