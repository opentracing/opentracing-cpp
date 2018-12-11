#include <opentracing/noop.h>
#include <opentracing/tracer.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
static std::shared_ptr<Tracer>& get_global_tracer() {
  static std::shared_ptr<Tracer> global_tracer = MakeNoopTracer();
  return global_tracer;
}

static std::atomic<bool>& get_is_global_tracer_registered() {
  static std::atomic<bool> is_global_tracer_registered{false};;
  return is_global_tracer_registered;
}

std::shared_ptr<Tracer> Tracer::Global() noexcept {
  return get_global_tracer();
}

std::shared_ptr<Tracer> Tracer::InitGlobal(
    std::shared_ptr<Tracer> tracer) noexcept {
  get_global_tracer().swap(tracer);
  get_is_global_tracer_registered().store(true);
  return tracer;
}

bool Tracer::IsGlobalTracerRegistered() noexcept {
  return get_is_global_tracer_registered().load();
}
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
