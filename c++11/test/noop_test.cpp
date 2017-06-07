#include <opentracing/noop.h>
#include <cassert>
using namespace opentracing;

int main() {
  auto tracer = make_noop_tracer();

  auto span1 = tracer->StartSpan("a");
  assert(&span1->tracer() == tracer.get());

  auto span2 = tracer->StartSpan("b", {ChildOf(span1->context())});
  span2->SetOperationName("b1");
  span2->SetTag("x", true);
  assert(span2->BaggageItem("y") == "");
  span2->Finish();

  return 0;
}
