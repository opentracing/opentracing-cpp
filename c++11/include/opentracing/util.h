#ifndef OPENTRACING_UTIL_H
#define OPENTRACING_UTIL_H

#include <opentracing/version.h>
#include <chrono>
#include <opentracing/martinmoene_expected/expected.hpp>
#include <system_error>

namespace opentracing {
inline namespace OPENTRACING_INLINE_NAMESPACE {
using SystemClock = std::chrono::system_clock;
using SteadyClock = std::chrono::steady_clock;
using SystemTime = SystemClock::time_point;
using SteadyTime = SteadyClock::time_point;

// Expected uses a C++11 implementation that follows the std::expected standard
// library proposal.
//
// See https://github.com/martinmoene/expected-lite
//     https://github.com/viboes/std-make/blob/master/doc/proposal/expected/d0323r2.md
template <class T, class E = std::error_code>
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

// Support conversion between time_points from different clocks. There's no
// standard way to get the difference in epochs between clocks, so this uses
// an approximation suggested by Howard Hinnant.
//
// See https://stackoverflow.com/a/35282833/4447365
template <class ToClock, class FromClock, class Duration>
typename ToClock::time_point convert_time_point(
    std::chrono::time_point<FromClock, Duration> from_time_point) {
  auto from_now = FromClock::now();
  auto to_now = ToClock::now();
  return to_now + std::chrono::duration_cast<typename ToClock::duration>(
                      from_time_point - from_now);
}
}  // namespace OPENTRACING_INLINE_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_UTIL_H
