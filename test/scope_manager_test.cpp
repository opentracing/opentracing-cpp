#include <opentracing/noop.h>
#include <opentracing/scope_manager.h>

using namespace opentracing;

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>

TEST_CASE("scope_manager") { CHECK(true); }
