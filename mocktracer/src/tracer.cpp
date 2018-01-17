#include <opentracing/mocktracer/tracer.h>
#include <cstdio>
#include <exception>

#include "mock_span.h"
#include "mock_span_context.h"

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

template <class Carrier>
static expected<void> InjectImpl(const opentracing::SpanContext& span_context,
                                 Carrier& writer) {
  auto mock_span_context = dynamic_cast<const MockSpanContext*>(&span_context);
  if (mock_span_context == nullptr) {
    return opentracing::make_unexpected(
        opentracing::invalid_span_context_error);
  }
  return mock_span_context->Inject(writer);
}

template <class Carrier>
opentracing::expected<std::unique_ptr<opentracing::SpanContext>> ExtractImpl(
    Carrier& reader) {
  MockSpanContext* mock_span_context;
  try {
    mock_span_context = new MockSpanContext{};
  } catch (const std::bad_alloc&) {
    return opentracing::make_unexpected(
        make_error_code(std::errc::not_enough_memory));
  }
  std::unique_ptr<opentracing::SpanContext> span_context(mock_span_context);
  auto result = mock_span_context->Extract(reader);
  if (!result) {
    return opentracing::make_unexpected(result.error());
  }
  if (!*result) {
    span_context.reset();
  }
  return std::move(span_context);
}

std::unique_ptr<Span> MockTracer::StartSpanWithOptions(
    string_view operation_name, const StartSpanOptions& options) const
    noexcept try {
  return std::unique_ptr<Span>{new MockSpan{shared_from_this(), recorder_.get(),
                                            operation_name, options}};
} catch (const std::exception& e) {
  fprintf(stderr, "Failed to start span: %s\n", e.what());
  return nullptr;
}

void MockTracer::Close() noexcept {
  if (recorder_ != nullptr) {
    recorder_->Close();
  }
}

expected<void> MockTracer::Inject(const SpanContext& sc,
                                  std::ostream& writer) const {
  return InjectImpl(sc, writer);
}

expected<void> MockTracer::Inject(const SpanContext& sc,
                                  const TextMapWriter& writer) const {
  return InjectImpl(sc, writer);
}

expected<void> MockTracer::Inject(const SpanContext& sc,
                                  const HTTPHeadersWriter& writer) const {
  return InjectImpl(sc, writer);
}

expected<std::unique_ptr<SpanContext>> MockTracer::Extract(
    std::istream& reader) const {
  return ExtractImpl(reader);
}

expected<std::unique_ptr<SpanContext>> MockTracer::Extract(
    const TextMapReader& reader) const {
  return ExtractImpl(reader);
}

expected<std::unique_ptr<SpanContext>> MockTracer::Extract(
    const HTTPHeadersReader& reader) const {
  return ExtractImpl(reader);
}
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
