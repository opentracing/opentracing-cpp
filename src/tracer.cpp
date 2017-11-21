#include <opentracing/noop.h>
#include <opentracing/tracer.h>
#include <sstream>
#include "in_memory_stream.h"

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
expected<void> Tracer::Inject(const SpanContext& sc,
                              std::string& writer) const {
  std::ostringstream oss;
  const auto result = Inject(sc, oss);
  if (!result) {
    return result;
  }
  if (!oss) {
    return opentracing::make_unexpected(
        std::make_error_code(std::errc::io_error));
  }
  try {
    writer = oss.str();
  } catch (const std::bad_alloc&) {
    return opentracing::make_unexpected(
        std::make_error_code(std::errc::not_enough_memory));
  }
  return result;
}

expected<std::unique_ptr<SpanContext>> Tracer::Extract(
    opentracing::string_view reader) const {
  // Use InMemoryStream to avoid having to copy the context.
  InMemoryStream istream{reader.data(), reader.size()};
  return Extract(istream);
}

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
