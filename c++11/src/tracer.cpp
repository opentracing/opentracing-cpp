#include <opentracing/noop.h>
#include <opentracing/tracer.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
static std::shared_ptr<Tracer>& get_global_tracer() {
  static std::shared_ptr<Tracer> global_tracer = MakeNoopTracer();
  return global_tracer;
}

std::shared_ptr<Tracer> Tracer::Global() noexcept {
  return get_global_tracer();
}

std::shared_ptr<Tracer> Tracer::InitGlobal(
    std::shared_ptr<Tracer> tracer) noexcept {
  get_global_tracer().swap(tracer);
  return tracer;
}
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
