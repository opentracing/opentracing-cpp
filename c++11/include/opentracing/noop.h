#ifndef OPENTRACING_NOOP_H
#define OPENTRACING_NOOP_H

#include <opentracing/preprocessor.h>
#include <opentracing/tracer.h>
#include <memory>

namespace opentracing {
inline namespace OPENTRACING_VERSION_NAMESPACE {
// A NoopTracer is a trivial, minimum overhead implementation of Tracer
// for which all operations are no-ops.
//
// The primary use of this implementation is in libraries, such as RPC
// frameworks, that make tracing an optional feature controlled by the
// end user. A no-op implementation allows said libraries to use it
// as the default Tracer and to write instrumentation that does
// not need to keep checking if the tracer instance is nil.
//
// For the same reason, the NoopTracer is the default "global" tracer
// (see Tracer::Global and Tracer::InitGlobal functions).
//
// WARNING: NoopTracer does not support baggage propagation.
std::shared_ptr<Tracer> make_noop_tracer() noexcept;
}  // namespace OPENTRACING_VERSION_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_NOOP_H