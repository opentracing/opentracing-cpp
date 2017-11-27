#include "assert.h"

#include <opentracing/noop.h>
#include <opentracing/tracer.h>
using namespace opentracing;

static void test_tracer_interface() {
  auto tracer = MakeNoopTracer();

  auto span1 = tracer->StartSpan("a");
  ASSERT(span1);
  ASSERT(&span1->tracer() == tracer.get());

  auto span2 = tracer->StartSpan("b", {ChildOf(&span1->context())});
  ASSERT(span2);
  span2->SetOperationName("b1");
  span2->SetTag("x", true);
  ASSERT(span2->BaggageItem("y").empty());
  span2->Log({{"event", "xyz"}, {"abc", 123}});
  span2->Finish();
}

static void test_start_span_options() {
  StartSpanOptions options;

  // A reference to null a SpanContext is ignored.
  ChildOf(nullptr).Apply(options);
  ASSERT(options.references.size() == 0);
}

int main() {
  test_tracer_interface();
  test_start_span_options();
  return 0;
}
