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

  SECTION(
      "Spans serialize/deserialize losslessly (provided Value(const char*) "
      "isn't used).") {
    {
      auto span_a = tracer->StartSpan("a");
      CHECK(span_a);
      span_a->SetBaggageItem("bag1", "123");
      span_a->SetBaggageItem("bag2", "455");

      auto span_b = tracer->StartSpan("b", {ChildOf(&span_a->context())});
      CHECK(span_b);
      span_b->SetTag("nullptr tag", nullptr);
      span_b->SetTag("string tag", std::string{"x"});
      span_b->SetTag("bool tag", true);
      span_b->SetTag("int tag", 123);
      span_b->SetTag("unsigned int tag", 123u);
      span_b->SetTag("double tag", 123.0);
      span_b->SetTag("vector tag", Values{123, 456});
      span_b->SetTag("dictionary tag", Dictionary{{"abc", 123}, {"xyz", 456}});
      span_b->SetTag("vector dictionary tag", Values{Dictionary{{"abc", 123}}});

      span_b->Log({{"k1", 1}, {"k2", 2}});
      span_b->Log({{"k3", 3}, {"k4", 4}});

      auto span_c = tracer->StartSpan(
          "c", {ChildOf(&span_a->context()), FollowsFrom(&span_b->context())});
      CHECK(span_c);
    }
    auto spans = recorder->spans();
    auto j = ToJson(spans);
    CHECK(!j.empty());
    auto spans_from_json = FromJson(j);
    CHECK(spans == spans_from_json);
  }
}
