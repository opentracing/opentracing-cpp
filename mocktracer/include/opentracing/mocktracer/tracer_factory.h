#ifndef OPENTRACING_MOCKTRACER_TRACER_FACTORY_H
#define OPENTRACING_MOCKTRACER_TRACER_FACTORY_H

#include <opentracing/mocktracer/symbols.h>
#include <opentracing/tracer_factory.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

class OPENTRACING_MOCK_TRACER_API MockTracerFactory : public TracerFactory {
 public:
  expected<std::shared_ptr<Tracer>> MakeTracer(const char* configuration,
                                               std::string& error_message) const
      noexcept override;
};

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_MOCKTRACER_TRACER_FACTORY_H
