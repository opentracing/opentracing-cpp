#include <opentracing/scope_manager.h>

using namespace opentracing;

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>

TEST_CASE("scope") {
  SECTION("Scope invokes callback on destruction") {
    int called = 0;
    {
      Scope scope{[&called]() { ++called; }};
      CHECK(called == 0);
    }
    CHECK(called == 1);
  }

  SECTION("Scope can be moved") {
    int called = 0;
    {
      Scope scope{[&called]() { ++called; }};
      { Scope scope2{std::move(scope)}; }
      CHECK(called == 1);
    }
    CHECK(called == 1);  // check for double calls
  }
}
