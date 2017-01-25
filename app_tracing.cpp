
#include <app_tracing.h>

namespace opentracing {
template<> MyHotTracer* MyHotTracer::s_tracer = 0;
}

