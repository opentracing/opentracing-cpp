#include "propagation.h"
#include <iostream>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {
static void WriteString(std::ostream& ostream, const std::string& s) {
  const uint32_t size = static_cast<uint32_t>(s.size());
  ostream.write(reinterpret_cast<const char*>(&size), sizeof(size));
  ostream.write(s.data(), size);
}

static void ReadString(std::istream& istream, std::string& s) {
  uint32_t size = 0;
  istream.read(reinterpret_cast<char*>(&size), sizeof(size));
  s.resize(size);
  istream.read(&s[0], size);
}

expected<void> InjectSpanContext(std::ostream& carrier,
                                 const SpanContextData& span_context_data) {
  carrier.write(reinterpret_cast<const char*>(&span_context_data.trace_id),
                sizeof(&span_context_data.trace_id));
  carrier.write(reinterpret_cast<const char*>(&span_context_data.span_id),
                sizeof(&span_context_data.span_id));
  const uint32_t num_baggage =
      static_cast<uint32_t>(span_context_data.baggage.size());
  carrier.write(reinterpret_cast<const char*>(&num_baggage),
                sizeof(num_baggage));
  for (auto& baggage_item : span_context_data.baggage) {
    WriteString(carrier, baggage_item.first);
    WriteString(carrier, baggage_item.second);
  }

  // Flush so that when we call carrier.good(), we'll get an accurate view of
  // the error state.
  carrier.flush();
  if (!carrier.good()) {
    return opentracing::make_unexpected(
        std::make_error_code(std::errc::io_error));
  }

  return {};
}

expected<bool> ExtractSpanContext(std::istream& carrier,
                                  SpanContextData& span_context_data) try {
  // istream::peek returns EOF if it's in an error state, so check for an error
  // state first before checking for an empty stream.
  if (!carrier.good()) {
    return opentracing::make_unexpected(
        std::make_error_code(std::errc::io_error));
  }

  // Check for the case when no span is encoded.
  if (carrier.peek() == EOF) {
    return false;
  }
  carrier.read(reinterpret_cast<char*>(&span_context_data.trace_id),
               sizeof(span_context_data.trace_id));
  carrier.read(reinterpret_cast<char*>(&span_context_data.span_id),
               sizeof(span_context_data.span_id));
  uint32_t num_baggage = 0;
  carrier.read(reinterpret_cast<char*>(&num_baggage), sizeof(num_baggage));
  std::string baggage_key, baggage_value;
  for (int i = 0; i < static_cast<int>(num_baggage); ++i) {
    ReadString(carrier, baggage_key);
    ReadString(carrier, baggage_value);
    span_context_data.baggage[baggage_key] = baggage_value;
    if (!carrier.good()) {
      return opentracing::make_unexpected(
          std::make_error_code(std::errc::io_error));
    }
  }

  return true;
} catch (const std::bad_alloc&) {
  return opentracing::make_unexpected(
      std::make_error_code(std::errc::not_enough_memory));
}

expected<void> InjectSpanContext(const TextMapWriter& carrier,
                                 const SpanContextData& span_context_data) {
  return {};
}

expected<bool> ExtractSpanContext(const TextMapReader& carrier,
                                  SpanContextData& span_context_data) {
  return false;
}

expected<void> InjectSpanContext(const HTTPHeadersWriter& carrier,
                                 const SpanContextData& span_context_data) {
  return {};
}

expected<bool> ExtractSpanContext(const HTTPHeadersReader& carrier,
                                  SpanContextData& span_context_data) {
  return false;
}
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
