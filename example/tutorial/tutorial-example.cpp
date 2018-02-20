// Demonstrates basic usage of the OpenTracing API. Uses OpenTracing's
// mocktracer to capture all the recorded spans as JSON.

#include <opentracing/mocktracer/json_recorder.h>
#include <opentracing/mocktracer/tracer.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "text_map_carrier.h"
using namespace opentracing;
using namespace opentracing::mocktracer;

int main() {
  MockTracerOptions options;
  std::unique_ptr<std::ostringstream> output{new std::ostringstream{}};
  std::ostringstream& oss = *output;
  options.recorder = std::unique_ptr<mocktracer::Recorder>{
      new JsonRecorder{std::move(output)}};

  std::shared_ptr<opentracing::Tracer> tracer{
      new MockTracer{std::move(options)}};

  auto parent_span = tracer->StartSpan("parent");
  assert(parent_span);

  // Create a child span.
  {
    auto child_span =
        tracer->StartSpan("childA", {ChildOf(&parent_span->context())});
    assert(child_span);

    // Set a simple tag.
    child_span->SetTag("simple tag", 123);

    // Set a complex tag.
    child_span->SetTag("complex tag",
                       Values{123, Dictionary{{"abc", 123}, {"xyz", 4.0}}});

    // Log simple values.
    child_span->Log({{"event", "simple log"}, {"abc", 123}});

    // Log complex values.
    child_span->Log({{"event", "complex log"},
                     {"data", Dictionary{{"a", 1}, {"b", Values{1, 2}}}}});

    child_span->Finish();
  }

  // Create a follows from span.
  {
    auto child_span =
        tracer->StartSpan("childB", {FollowsFrom(&parent_span->context())});

    // child_span's destructor will finish the span if not done so explicitly.
  }

  // Use custom timestamps.
  {
    auto t1 = SystemClock::now();
    auto t2 = SteadyClock::now();
    auto span = tracer->StartSpan(
        "useCustomTimestamps",
        {ChildOf(&parent_span->context()), StartTimestamp(t1)});
    assert(span);
    span->Finish({FinishTimestamp(t2)});
  }

  // Extract and Inject a span context.
  {
    std::unordered_map<std::string, std::string> text_map;
    TextMapCarrier carrier(text_map);
    auto err = tracer->Inject(parent_span->context(), carrier);
    assert(err);
    auto span_context_maybe = tracer->Extract(carrier);
    assert(span_context_maybe);
    auto span = tracer->StartSpan("propagationSpan",
                                  {ChildOf(span_context_maybe->get())});
  }

  // You get an error when trying to extract a corrupt span.
  {
    std::unordered_map<std::string, std::string> text_map = {
        {"x-ot-span-context", "123"}};
    TextMapCarrier carrier(text_map);
    auto err = tracer->Extract(carrier);
    assert(!err);
    assert(err.error() == span_context_corrupted_error);
    // How to get a readable message from the error.
    std::cout << "Example error message: \"" << err.error().message() << "\"\n";
  }

  parent_span->Finish();
  tracer->Close();

  std::cout << "\nRecorded spans as JSON:\n\n";
  std::cout << oss.str() << "\n";
  return 0;
}
