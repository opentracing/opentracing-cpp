#ifndef OPENTRACING_UTIL_H
#define OPENTRACING_UTIL_H

#include <opentracing/string_view.h>
#include <opentracing/version.h>
#include <chrono>
#include <system_error>

// expected uses a C++11 implementation that follows the std::expected standard
// library proposal.
//
// See https://github.com/martinmoene/expected-lite
//     https://github.com/viboes/std-make/blob/master/doc/proposal/expected/d0323r2.md
#include <opentracing/expected/expected.hpp>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
using SystemClock = std::chrono::system_clock;
using SteadyClock = std::chrono::steady_clock;
using SystemTime = SystemClock::time_point;
using SteadyTime = SteadyClock::time_point;

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
template <class ToClock, class FromClock, class Duration,
          typename std::enable_if<
              !std::is_same<FromClock, ToClock>::value>::type * = nullptr>
typename ToClock::time_point convert_time_point(
    std::chrono::time_point<FromClock, Duration> from_time_point) {
  auto from_now = FromClock::now();
  auto to_now = ToClock::now();
  return to_now + std::chrono::duration_cast<typename ToClock::duration>(
                      from_time_point - from_now);
}

template <class ToClock, class FromClock, class Duration,
          typename std::enable_if<std::is_same<FromClock, ToClock>::value>::type
              * = nullptr>
typename ToClock::time_point convert_time_point(
    std::chrono::time_point<FromClock, Duration> from_time_point) {
  return std::chrono::time_point_cast<typename ToClock::time_point::duration>(
      from_time_point);
}

// std::error_code's have default comparison operators; however, they make use
// of singleton addresses which can cause comparisons to fail when multiple
// versions of the opentracing library are linked in. Since this is a common
// deployment scenario when making OpenTracing plugins, we add this utility
// function to make comparing std::error_code across libraries easier.
//
// Note: There's a proposed change to the C++ standard that addresses this
// issue. See
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1196r0.html
inline bool are_errors_equal(std::error_code lhs,
                             std::error_code rhs) noexcept {
  return opentracing::string_view{lhs.category().name()} ==
             opentracing::string_view{rhs.category().name()} &&
         lhs.value() == rhs.value();
}

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_UTIL_H
