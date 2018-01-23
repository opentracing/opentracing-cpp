#include <opentracing/mocktracer/json_recorder.h>
#include <opentracing/mocktracer/tracer.h>
#include <opentracing/mocktracer/tracer_factory.h>
#include <fstream>
#include <opentracing/mocktracer/nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

struct MockTracerConfiguration {
  std::string output_file;
};

template <>
struct adl_serializer<MockTracerConfiguration> {
  static void from_json(json& j, MockTracerConfiguration& configuration) {
    configuration.output_file = j["output_file"];
  }
};

expected<std::shared_ptr<Tracer>> MockTracerFactory::MakeTracer(
    const char* configuration, std::string& error_message) const noexcept try {
  MockTracerConfiguration tracer_configuration;
  json j;

  try {
    j = json::parse(configuration);
  } catch (const std::exception& e) {
    error_message = e.what();
    return make_unexpected(configuration_parse_error);
  }

  try {
    tracer_configuration = MockTracerConfiguration{j};
  } catch (const std::exception& e) {
    error_message = e.what();
    return make_unexpected(invalid_configuration_error);
  }

  std::unique_ptr<std::ostream> ostream{
      new std::ofstream{tracer_configuration.output_file}};
  if (!ostream->good()) {
    error_message = "failed to open file `";
    error_message += tracer_configuration.output_file + "`";  
    return make_unexpected(invalid_configuration_error);
  }

  MockTracerOptions tracer_options;
  tracer_options.recorder = std::unique_ptr<Recorder>{
    new JsonRecorder{std::move(ostream)}
  };

  return std::shared_ptr<Tracer>{new MockTracer{std::move(tracer_options)}};
} catch (const std::bad_alloc&) {
    return make_unexpected(std::make_error_code(std::errc::not_enough_memory));
}

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
