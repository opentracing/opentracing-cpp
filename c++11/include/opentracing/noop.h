#ifndef OPENTRACING_NOOP_H
#define OPENTRACING_NOOP_H

#include <opentracing/tracer.h>
#include <memory>

namespace opentracing {
std::shared_ptr<Tracer> make_noop_tracer();
}  // namespace opentracing

#endif  // OPENTRACING_NOOP_H
