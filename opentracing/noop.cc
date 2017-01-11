#include <opentracing/noop.h>

namespace opentracing {
NoopTracer* NoopTracer::s_tracer = 0;
}
