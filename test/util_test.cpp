#include <opentracing/util.h>
#include <cmath>
#include <cstdlib>  // Work around to https://stackoverflow.com/a/30084734.
using namespace opentracing;

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>

TEST_CASE("convert_time_point") {
  SECTION("We can convert between time points of different clocks") {
    // Check that converting from a system_clock time_point to a steady_clock
    // time point and then back again produces approximately the same
    // system_clock time_point.
    auto t1 = SystemClock::now();
    auto t2 = convert_time_point<SteadyClock>(t1);
    auto t3 = convert_time_point<SystemClock>(t2);
    auto difference = std::abs(
        std::chrono::duration_cast<std::chrono::microseconds>(t3 - t1).count());
    CHECK(difference < 100);
  }

  SECTION("Converting times from the same clock gives the identity") {
    auto t = SystemClock::now();
    CHECK(t == convert_time_point<SystemClock>(t));
  }
}
