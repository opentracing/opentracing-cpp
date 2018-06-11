#include <opentracing/mocktracer/in_memory_recorder.h>
#include <opentracing/mocktracer/json.h>
#include <opentracing/mocktracer/json_recorder.h>
#include <opentracing/mocktracer/tracer.h>
#include <opentracing/noop.h>
#include <sstream>

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>
using namespace opentracing;
using namespace mocktracer;

TEST_CASE("tracer") {
  auto recorder = new InMemoryRecorder{};
  MockTracerOptions tracer_options;
  tracer_options.recorder.reset(recorder);
  auto tracer = std::shared_ptr<opentracing::Tracer>{
      new MockTracer{std::move(tracer_options)}};

  SECTION("MockTracer can be constructed without a recorder.") {
    auto norecorder_tracer = std::shared_ptr<opentracing::Tracer>{
        new MockTracer{MockTracerOptions{}}};
    auto span = norecorder_tracer->StartSpan("a");
  }

  SECTION("StartSpan applies the provided tags.") {
    {
      auto span =
          tracer->StartSpan("a", {SetTag("abc", 123), SetTag("xyz", true)});
      CHECK(span);
      span->Finish();
    }
    auto span = recorder->top();
    CHECK(span.operation_name == "a");
    std::map<std::string, Value> expected_tags = {{"abc", 123}, {"xyz", true}};
    CHECK(span.tags == expected_tags);
  }

  SECTION("You can set a single child-of reference when starting a span.") {
    auto span_a = tracer->StartSpan("a");
    CHECK(span_a);
    span_a->Finish();
    auto span_b = tracer->StartSpan("b", {ChildOf(&span_a->context())});
    CHECK(span_b);
    span_b->Finish();
    auto spans = recorder->spans();
    CHECK(spans.at(0).span_context.trace_id ==
          spans.at(1).span_context.trace_id);
    std::vector<SpanReferenceData> expected_references = {
        {SpanReferenceType::ChildOfRef, spans.at(0).span_context.trace_id,
         spans.at(0).span_context.span_id}};
    CHECK(spans.at(1).references == expected_references);
  }

  SECTION("You can set a single follows-from reference when starting a span.") {
    auto span_a = tracer->StartSpan("a");
    CHECK(span_a);
    span_a->Finish();
    auto span_b = tracer->StartSpan("b", {FollowsFrom(&span_a->context())});
    CHECK(span_b);
    span_b->Finish();
    auto spans = recorder->spans();
    CHECK(spans.at(0).span_context.trace_id ==
          spans.at(1).span_context.trace_id);
    std::vector<SpanReferenceData> expected_references = {
        {SpanReferenceType::FollowsFromRef, spans.at(0).span_context.trace_id,
         spans.at(0).span_context.span_id}};
    CHECK(spans.at(1).references == expected_references);
  }

  SECTION("Multiple references are supported when starting a span.") {
    auto span_a = tracer->StartSpan("a");
    CHECK(span_a);
    auto span_b = tracer->StartSpan("b");
    CHECK(span_b);
    auto span_c = tracer->StartSpan(
        "c", {ChildOf(&span_a->context()), FollowsFrom(&span_b->context())});
    span_a->Finish();
    span_b->Finish();
    span_c->Finish();
    auto spans = recorder->spans();
    std::vector<SpanReferenceData> expected_references = {
        {SpanReferenceType::ChildOfRef, spans.at(0).span_context.trace_id,
         spans.at(0).span_context.span_id},
        {SpanReferenceType::FollowsFromRef, spans.at(1).span_context.trace_id,
         spans.at(1).span_context.span_id}};
    CHECK(spans.at(2).references == expected_references);
  }

  SECTION(
      "Baggage from the span references are copied over to a new span "
      "context") {
    auto span_a = tracer->StartSpan("a");
    CHECK(span_a);
    span_a->SetBaggageItem("a", "1");
    auto span_b = tracer->StartSpan("b");
    CHECK(span_b);
    span_b->SetBaggageItem("b", "2");
    auto span_c = tracer->StartSpan(
        "c", {ChildOf(&span_a->context()), ChildOf(&span_b->context())});
    CHECK(span_c);
    CHECK(span_c->BaggageItem("a") == "1");
    CHECK(span_c->BaggageItem("b") == "2");
  }

  SECTION("References to non-MockTracer spans and null pointers are ignored.") {
    auto noop_tracer = MakeNoopTracer();
    auto noop_span = noop_tracer->StartSpan("noop");
    CHECK(noop_span);
    StartSpanOptions options;
    options.references.push_back(
        std::make_pair(SpanReferenceType::ChildOfRef, &noop_span->context()));
    options.references.push_back(
        std::make_pair(SpanReferenceType::ChildOfRef, nullptr));
    auto span = tracer->StartSpanWithOptions("a", options);
    CHECK(span);
    span->Finish();
    CHECK(recorder->top().references.size() == 0);
  }

  SECTION("Calling Finish a second time does nothing.") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->Finish();
    CHECK(recorder->size() == 1);
    span->Finish();
    CHECK(recorder->size() == 1);
  }

  SECTION("FinishWithOptions applies provided log records.") {
    std::vector<LogRecord> logs;
    {
      auto span = tracer->StartSpan("a");
      CHECK(span);
      FinishSpanOptions options;
      auto timestamp = SystemClock::now();
      logs = {{timestamp, {{"abc", 123}}}};
      options.log_records = logs;
      span->FinishWithOptions(options);
    }
    auto span = recorder->top();
    CHECK(span.operation_name == "a");
    CHECK(span.logs == logs);
  }

  SECTION("Logs can be added to an active span.") {
    {
      auto span = tracer->StartSpan("a");
      CHECK(span);
      span->Log({{"abc", 123}});
    }
    auto span = recorder->top();
    std::vector<std::pair<std::string, Value>> fields = {{"abc", 123}};
    CHECK(span.logs.at(0).fields == fields);
  }

  SECTION("The operation name can be changed after the span is started.") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->SetOperationName("b");
    span->Finish();
    CHECK(recorder->top().operation_name == "b");
  }

  SECTION("Tags can be specified after a span is started.") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->SetTag("abc", 123);
    span->Finish();
    std::map<std::string, Value> expected_tags = {{"abc", 123}};
    CHECK(recorder->top().tags == expected_tags);
  }
}

TEST_CASE("json_recorder") {
  auto oss = new std::ostringstream{};
  MockTracerOptions tracer_options;
  tracer_options.recorder = std::unique_ptr<Recorder>{
      new JsonRecorder{std::unique_ptr<std::ostream>{oss}}};
  auto tracer =
      std::shared_ptr<Tracer>{new MockTracer{std::move(tracer_options)}};

  SECTION("Spans are serialized to the stream upon Close.") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->Finish();
    tracer->Close();
    CHECK(!oss->str().empty());
  }
}
