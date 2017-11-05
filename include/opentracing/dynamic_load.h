#ifndef OPENTRACING_DYNAMIC_LOAD_H
#define OPENTRACING_DYNAMIC_LOAD_H

#include <opentracing/tracer.h>
#include <system_error>

extern "C" int __attribute((weak))
opentracing_make_vendor_tracer(const char* opentracing_version,
                               const char* json_configuration,
                               const char** error_message,
                               void* tracer_shared_ptr);

namespace opentracing {
const std::error_category& dynamic_load_error_category();

const std::error_code library_not_found_error(1,
    dynamic_load_error_category());

const std::error_code dynamic_load_not_supported_error(2,
    dynamic_load_error_category());

const std::error_code incompatible_library_versions_error(3,
    dynamic_load_error_category());

const std::error_code configuration_parse_error(4,
    dynamic_load_error_category());

const std::error_code invalid_configuration_error(5,
    dynamic_load_error_category());

const std::error_code internal_tracer_error(6,
    dynamic_load_error_category());

expected<std::shared_ptr<Tracer>> dynamically_load_tracer(
    const char* shared_library, const char* json_configuration,
    std::string& error_message) noexcept;
}  // namespace opentracing
#endif // OPENTRACING_DYNAMIC_LOAD_H
