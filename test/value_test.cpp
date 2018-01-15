#include <opentracing/value.h>
using namespace opentracing;

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>

TEST_CASE("Value") {
  SECTION("Signed integers get converted to int64_t.") {
    Value v1(123);
    CHECK(v1.is<int64_t>());

    Value v2(static_cast<short>(123));
    CHECK(v2.is<int64_t>());
  }

  SECTION("Unsigned integers get converted to uint64_t.") {
    Value v1(123u);
    CHECK(v1.is<uint64_t>());

    Value v2(static_cast<unsigned short>(123));
    CHECK(v2.is<uint64_t>());
  }

  SECTION("Bool values are deduced as bool.") {
    Value v1(true);
    // Workaround for "disabled expansion of recursive macro" warning.
    const auto is_bool = v1.is<bool>();
    CHECK(is_bool);
  }

  SECTION("Floating point numbers are converted to double.") {
    Value v1(1.0);
    CHECK(v1.is<double>());
    Value v2(1.0f);
    CHECK(v2.is<double>());
  }

  SECTION("std::string values are deduced as std::string.") {
    Value v1(std::string("abc"));
    CHECK(v1.is<std::string>());
  }

  SECTION("c-string values are deduced as c-strings.") {
    Value v1("abc");
    CHECK(v1.is<const char*>());
  }

  SECTION("Complex values are permitted.") {
    Value v1(Values{Value(1), Value(2)});
    (void)v1;

    Value v2(Dictionary{{"abc", Value(123)}});
    (void)v2;
  }

  SECTION("Value types can be compared for equality.") {
    Value v1{1}, v2{2}, v3{1.0};
    CHECK(v1 == v1);
    CHECK(v1 != v2);
    CHECK(v1 != v3);
  }
}
