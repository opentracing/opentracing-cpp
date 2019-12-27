#include <opentracing/thread_local_scope_manager.h>

using namespace opentracing;

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>

TEST_CASE("thread_local_scope_manager") { CHECK(true); }
