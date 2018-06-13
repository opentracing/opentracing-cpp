#ifndef OPENTRACING_TRACER_FACTORY_H
#define OPENTRACING_TRACER_FACTORY_H

#include <opentracing/symbols.h>
#include <opentracing/tracer.h>
#include <opentracing/version.h>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
// Returns the std::error_category class used for tracer factory errors.
//
// See
//   http://blog.think-async.com/2010/04/system-error-support-in-c0x-part-1.html
//   https://ned14.github.io/boost.outcome/md_doc_md_03-tutorial_b.html
OPENTRACING_API const std::error_category& tracer_factory_error_category();

// `configuration_parse_error` occurs when the configuration string used to
// construct a tracer does not adhere to the expected format.
const std::error_code configuration_parse_error(
    1, tracer_factory_error_category());

// `invalid_configuration_error` occurs if the requested configuration for a
// tracer has invalid values.
const std::error_code invalid_configuration_error(
    2, tracer_factory_error_category());

// TracerFactory constructs tracers from configuration strings.
class OPENTRACING_API TracerFactory {
 public:
  virtual ~TracerFactory() = default;

  // Creates a tracer with the requested `configuration`.
  //
  // Example,
  //   const char* configuration = R"(
  //      "collector": "localhost:123",
  //      "max_buffered_spans": 500
  //   )";
  //   std:string error_message;
  //   auto tracer_maybe = tracer_factory->MakeTracer(configuration,
  //                                                  error_message);
  //   if (tracer_mabye) {
  //      // success
  //      std::shared_ptr<opentracing::Tracer> tracer = *tracer_maybe;
  //   } else {
  //      // failure
  //      std::error_code error = tracer_maybe.error();
  //      // `error_message` may also contain a more descriptive message
  //   }
  virtual expected<std::shared_ptr<Tracer>> MakeTracer(
      const char* configuration, std::string& error_message) const noexcept = 0;
};
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing

#endif  // OPENTRACING_TRACER_FACTORY_H
