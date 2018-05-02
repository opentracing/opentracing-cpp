#include <opentracing/string_view.h>  // test include guard
#include <opentracing/string_view.h>

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>

using namespace opentracing;

TEST_CASE("string_view") {
  SECTION("A default-constructed string_view is empty.") {
    string_view ref;
    CHECK(nullptr == ref.data());
    CHECK(0 == ref.length());
  }

  SECTION("string_view can be initialized from a c-string.") {
    const char* val = "hello world";

    string_view ref(val);

    CHECK(val == ref.data());
    CHECK(std::strlen(val) == ref.length());
  }

  SECTION("string_view can be initialized from an std::string.") {
    const std::string val = "hello world";

    string_view ref(val);

    CHECK(val == ref.data());
    CHECK(val.length() == ref.length());
  }

  SECTION("A copied string_view points to the same data as its source.") {
    const std::string val = "hello world";

    string_view ref(val);
    string_view cpy(ref);

    CHECK(val == cpy.data());
    CHECK(val.length() == cpy.length());
  }

  SECTION("operator[] can be used to access characters in a string_view") {
    string_view s = "abc123";
    CHECK(&s[0] == s.data());
    CHECK(&s[1] == s.data() + 1);
  }
}
