#include "propagation.h"
#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <sstream>
#include "base64.h"
#include "utility.h"

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

expected<void> InjectSpanContext(
    const PropagationOptions& /*propagation_options*/, std::ostream& carrier,
    const SpanContextData& span_context_data) {
  auto trace_id = SwapEndianIfBig(span_context_data.trace_id);
  carrier.write(reinterpret_cast<const char*>(&trace_id), sizeof(trace_id));
  auto span_id = SwapEndianIfBig(span_context_data.span_id);
  carrier.write(reinterpret_cast<const char*>(&span_id), sizeof(span_id));

  const uint32_t num_baggage =
      SwapEndianIfBig(static_cast<uint32_t>(span_context_data.baggage.size()));
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

expected<bool> ExtractSpanContext(
    const PropagationOptions& /*propagation_options*/, std::istream& carrier,
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
  span_context_data.trace_id = SwapEndianIfBig(span_context_data.trace_id);
  carrier.read(reinterpret_cast<char*>(&span_context_data.span_id),
               sizeof(span_context_data.span_id));
  span_context_data.span_id = SwapEndianIfBig(span_context_data.span_id);
  uint32_t num_baggage = 0;
  carrier.read(reinterpret_cast<char*>(&num_baggage), sizeof(num_baggage));
  num_baggage = SwapEndianIfBig(num_baggage);
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

expected<void> InjectSpanContext(const PropagationOptions& propagation_options,
                                 const TextMapWriter& carrier,
                                 const SpanContextData& span_context_data) {
  std::ostringstream ostream;
  auto result =
      InjectSpanContext(propagation_options, ostream, span_context_data);
  if (!result) {
    return result;
  }
  std::string context_value;
  try {
    auto binary_encoding = ostream.str();
    context_value =
        Base64::encode(binary_encoding.data(), binary_encoding.size());
  } catch (const std::bad_alloc&) {
    return opentracing::make_unexpected(
        std::make_error_code(std::errc::not_enough_memory));
  }

  result = carrier.Set(propagation_options.propagation_key, context_value);
  if (!result) {
    return result;
  }
  return {};
}

template <class KeyCompare>
static opentracing::expected<opentracing::string_view> LookupKey(
    const opentracing::TextMapReader& carrier, opentracing::string_view key,
    KeyCompare key_compare) {
  // First try carrier.LookupKey since that can potentially be the fastest
  // approach.
  auto result = carrier.LookupKey(key);
  if (result ||
      !are_errors_equal(result.error(),
                        opentracing::lookup_key_not_supported_error)) {
    return result;
  }

  // Fall back to iterating through all of the keys.
  result = opentracing::make_unexpected(opentracing::key_not_found_error);
  auto was_successful = carrier.ForeachKey(
      [&](opentracing::string_view carrier_key,
          opentracing::string_view value) -> opentracing::expected<void> {
        if (!key_compare(carrier_key, key)) {
          return {};
        }
        result = value;

        // Found key, so bail out of the loop with a success error code.
        return opentracing::make_unexpected(std::error_code{});
      });
  if (!was_successful && was_successful.error() != std::error_code{}) {
    return opentracing::make_unexpected(was_successful.error());
  }
  return result;
}

template <class KeyCompare>
static opentracing::expected<bool> ExtractSpanContext(
    const PropagationOptions& propagation_options,
    const opentracing::TextMapReader& carrier,
    SpanContextData& span_context_data, KeyCompare key_compare) {
  auto value_maybe =
      LookupKey(carrier, propagation_options.propagation_key, key_compare);
  if (!value_maybe) {
    if (are_errors_equal(value_maybe.error(),
                         opentracing::key_not_found_error)) {
      return false;
    } else {
      return opentracing::make_unexpected(value_maybe.error());
    }
  }
  auto value = *value_maybe;
  std::string base64_decoding;
  try {
    base64_decoding = Base64::decode(value.data(), value.size());
  } catch (const std::bad_alloc&) {
    return opentracing::make_unexpected(
        std::make_error_code(std::errc::not_enough_memory));
  }
  if (base64_decoding.empty()) {
    return opentracing::make_unexpected(
        opentracing::span_context_corrupted_error);
  }
  std::istringstream istream{base64_decoding};
  return ExtractSpanContext(propagation_options, istream, span_context_data);
}

expected<bool> ExtractSpanContext(const PropagationOptions& propagation_options,
                                  const TextMapReader& carrier,
                                  SpanContextData& span_context_data) {
  return ExtractSpanContext(propagation_options, carrier, span_context_data,
                            std::equal_to<string_view>{});
}

expected<void> InjectSpanContext(const PropagationOptions& propagation_options,
                                 const HTTPHeadersWriter& carrier,
                                 const SpanContextData& span_context_data) {
  return InjectSpanContext(propagation_options,
                           static_cast<const TextMapWriter&>(carrier),
                           span_context_data);
}

expected<bool> ExtractSpanContext(const PropagationOptions& propagation_options,
                                  const HTTPHeadersReader& carrier,
                                  SpanContextData& span_context_data) {
  auto iequals = [](opentracing::string_view lhs,
                    opentracing::string_view rhs) {
    return lhs.length() == rhs.length() &&
           std::equal(std::begin(lhs), std::end(lhs), std::begin(rhs),
                      [](char a, char b) {
                        return std::tolower(a) == std::tolower(b);
                      });
  };
  return ExtractSpanContext(propagation_options, carrier, span_context_data,
                            iequals);
}
}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
