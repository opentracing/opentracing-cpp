#include <opentracing/mocktracer/in_memory_recorder.h>
#include <opentracing/mocktracer/tracer.h>
#include <opentracing/mocktracer/json.h>

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>
using namespace opentracing;
using namespace mocktracer;

TEST_CASE("json") {
  auto recorder = new InMemoryRecorder{};
  MockTracerOptions tracer_options;
  tracer_options.recorder.reset(recorder);
  auto tracer = std::shared_ptr<opentracing::Tracer>{
      new MockTracer{std::move(tracer_options)}};

  {
    auto span_a = tracer->StartSpan("a");
    CHECK(span_a);

    auto span_b = tracer->StartSpan("b", {ChildOf(&span_a->context())});
    CHECK(span_b);
    span_b->SetTag("abc", 123);
    span_b->Log({{"xyz", 321}});
  }
  auto j = ToJson(recorder->spans());
  CHECK(!j.empty());
}
