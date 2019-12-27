#include <opentracing/noop.h>
#include <opentracing/thread_local_scope_manager.h>
#include <memory>
#include <thread>

using namespace opentracing;

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>

TEST_CASE("thread_local_scope_manager") {
  ThreadLocalScopeManager sm;
  std::shared_ptr<Span> default_span;

  SECTION("Returns noop span with no activations") {
    default_span = sm.ActiveSpan();
    CHECK(default_span);
  }

  auto tracer = MakeNoopTracer();

  SECTION("Basic span activation/deactivation") {
    std::shared_ptr<Span> span{tracer->StartSpan("a")};
    CHECK(sm.ActiveSpan() == default_span);
    {
      auto scope = sm.Activate(span);
      CHECK(sm.ActiveSpan() == span);
    }
    CHECK(sm.ActiveSpan() == default_span);
  }

  SECTION("Nested span activation/deactivation") {
    std::shared_ptr<Span> span1{tracer->StartSpan("1")};
    std::shared_ptr<Span> span2{tracer->StartSpan("2")};
    CHECK(sm.ActiveSpan() == default_span);
    {
      auto scope1 = sm.Activate(span1);
      CHECK(sm.ActiveSpan() == span1);
      {
        auto scope2 = sm.Activate(span2);
        CHECK(sm.ActiveSpan() == span2);
      }
      CHECK(sm.ActiveSpan() == span1);
    }
    CHECK(sm.ActiveSpan() == default_span);
  }

  SECTION("Validate span activation is local to thread") {
    std::shared_ptr<Span> thread_span;
    std::shared_ptr<Span> span{tracer->StartSpan("a")};
    auto scope = sm.Activate(span);

    // Start thread which captures active span
    std::thread{[&thread_span, &sm]() {
      thread_span = sm.ActiveSpan();
    }}.join();

    CHECK(thread_span != span);
  }
}
