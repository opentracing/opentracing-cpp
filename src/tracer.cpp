#include <opentracing/noop.h>
#include <opentracing/tracer.h>

#include <mutex>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace {
class TracerRegistry {
 public:
  static TracerRegistry& instance() noexcept {
    static TracerRegistry result;
    return result;
  }

  static std::shared_ptr<Tracer> RegisterTracer(
      std::shared_ptr<Tracer>& tracer) noexcept {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    is_registered_ = true;
    tracer_.swap(tracer);
    return tracer;
  }

  static std::shared_ptr<Tracer> tracer() noexcept {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    return tracer_;
  }

  static bool is_registered() noexcept {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    return is_registered_;
  }

 private:
  static std::mutex mutex_;
  static bool is_registered_;
  static std::shared_ptr<Tracer> tracer_;
};

std::mutex TracerRegistry::mutex_;

bool TracerRegistry::is_registered_{false};

std::shared_ptr<Tracer> TracerRegistry::tracer_{MakeNoopTracer()};
}  // namespace

std::shared_ptr<Tracer> Tracer::Global() noexcept {
  return TracerRegistry::instance().tracer();
}

std::shared_ptr<Tracer> Tracer::InitGlobal(
    std::shared_ptr<Tracer> tracer) noexcept {
  return TracerRegistry::instance().RegisterTracer(tracer);
}

bool Tracer::IsGlobalTracerRegistered() noexcept {
  return TracerRegistry::instance().is_registered();
}
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
