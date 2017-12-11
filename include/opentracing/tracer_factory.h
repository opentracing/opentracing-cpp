#ifndef OPENTRACING_TRACER_FACTORY_H
#define OPENTRACING_TRACER_FACTORY_H

#include <opentracing/tracer.h>
#include <opentracing/version.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE

const std::error_category& tracer_factory_error_category();

const std::error_code configuration_parse_error(
    1, tracer_factory_error_category());

const std::error_code invalid_configuration_error(
    2, tracer_factory_error_category());

class TracerFactory {
 public:
  virtual ~TracerFactory() = default;

  virtual expected<std::shared_ptr<Tracer>> MakeTracer(
      const char* configuration, std::string& error_message) const noexcept = 0;
};

END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_TRACER_FACTORY_H
