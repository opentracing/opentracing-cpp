#include <opentracing/mocktracer/in_memory_recorder.h>
#include <opentracing/mocktracer/tracer.h>
#include <opentracing/noop.h>
#include <sstream>
#include <string>
#include <unordered_map>

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>
using namespace opentracing;
using namespace mocktracer;

struct TextMapCarrier : TextMapReader, TextMapWriter {
  TextMapCarrier(std::unordered_map<std::string, std::string>& text_map_)
      : text_map(text_map_) {}

  expected<void> Set(string_view key, string_view value) const override {
    text_map[key] = value;
    return {};
  }

  expected<string_view> LookupKey(string_view key) const override {
    if (!supports_lookup) {
      return make_unexpected(lookup_key_not_supported_error);
    }
    auto iter = text_map.find(key);
    if (iter != text_map.end()) {
      return string_view{iter->second};
    } else {
      return make_unexpected(key_not_found_error);
    }
  }

  expected<void> ForeachKey(
      std::function<expected<void>(string_view key, string_view value)> f)
      const override {
    ++foreach_key_call_count;
    for (const auto& key_value : text_map) {
      auto result = f(key_value.first, key_value.second);
      if (!result) return result;
    }
    return {};
  }

  bool supports_lookup = false;
  mutable int foreach_key_call_count = 0;
  std::unordered_map<std::string, std::string>& text_map;
};

struct HTTPHeadersCarrier : HTTPHeadersReader, HTTPHeadersWriter {
  HTTPHeadersCarrier(std::unordered_map<std::string, std::string>& text_map_)
      : text_map(text_map_) {}

  expected<void> Set(string_view key, string_view value) const override {
    text_map[key] = value;
    return {};
  }

  expected<void> ForeachKey(
      std::function<expected<void>(string_view key, string_view value)> f)
      const override {
    for (const auto& key_value : text_map) {
      auto result = f(key_value.first, key_value.second);
      if (!result) return result;
    }
    return {};
  }

  std::unordered_map<std::string, std::string>& text_map;
};

TEST_CASE("propagation") {
  const char* propagation_key = "propagation-key";
  auto recorder = new InMemoryRecorder{};
  MockTracerOptions tracer_options;
  tracer_options.propagation_options.propagation_key = propagation_key;
  tracer_options.recorder.reset(recorder);
  auto tracer = std::shared_ptr<opentracing::Tracer>{
      new MockTracer{std::move(tracer_options)}};
  std::unordered_map<std::string, std::string> text_map;
  TextMapCarrier text_map_carrier(text_map);
  HTTPHeadersCarrier http_headers_carrier(text_map);
  auto span = tracer->StartSpan("a");
  CHECK(span);
  span->SetBaggageItem("abc", "123");

  SECTION("Propagation uses the specified propagation_key.") {
    CHECK(tracer->Inject(span->context(), text_map_carrier));
    CHECK(text_map.count(propagation_key) == 1);
  }

  SECTION("Inject, extract, inject yields the same text_map.") {
    CHECK(tracer->Inject(span->context(), text_map_carrier));
    auto injection_map1 = text_map;
    auto span_context_maybe = tracer->Extract(text_map_carrier);
    CHECK((span_context_maybe && span_context_maybe->get()));
    text_map.clear();
    CHECK(tracer->Inject(*span_context_maybe->get(), text_map_carrier));
    CHECK(injection_map1 == text_map);
  }

  SECTION("Inject, extract, inject yields the same binary blob.") {
    std::ostringstream oss(std::ios::binary);
    CHECK(tracer->Inject(span->context(), oss));
    auto blob = oss.str();
    std::istringstream iss(blob, std::ios::binary);
    auto span_context_maybe = tracer->Extract(iss);
    CHECK((span_context_maybe && span_context_maybe->get()));
    std::ostringstream oss2(std::ios::binary);
    CHECK(tracer->Inject(*span_context_maybe->get(), oss2));
    CHECK(blob == oss2.str());
  }

  SECTION(
      "Extracing a context from an empty text-map gives a null span context.") {
    auto span_context_maybe = tracer->Extract(text_map_carrier);
    CHECK(span_context_maybe);
    CHECK(span_context_maybe->get() == nullptr);
  }

  SECTION("Injecting a non-Mock span returns invalid_span_context_error.") {
    auto noop_tracer = opentracing::MakeNoopTracer();
    CHECK(noop_tracer);
    auto noop_span = noop_tracer->StartSpan("a");
    CHECK(noop_span);
    auto was_successful =
        tracer->Inject(noop_span->context(), text_map_carrier);
    CHECK(!was_successful);
    CHECK(was_successful.error() == opentracing::invalid_span_context_error);
  }

  SECTION("Extract is insensitive to changes in case for http header fields") {
    CHECK(tracer->Inject(span->context(), http_headers_carrier));

    // Change the case of one of the fields.
    auto key_value = *std::begin(text_map);
    text_map.erase(std::begin(text_map));
    auto key = key_value.first;
    key[0] = key[0] == std::toupper(key[0])
                 ? static_cast<char>(std::tolower(key[0]))
                 : static_cast<char>(std::toupper(key[0]));
    text_map[key] = key_value.second;
    CHECK(tracer->Extract(http_headers_carrier));
  }

  SECTION("Extract/Inject fail if a stream has failure bits set.") {
    std::ostringstream oss(std::ios::binary);
    oss.setstate(std::ios_base::failbit);
    CHECK(!tracer->Inject(span->context(), oss));
    oss.clear();
    CHECK(tracer->Inject(span->context(), oss));
    auto blob = oss.str();
    std::istringstream iss(blob, std::ios::binary);
    iss.setstate(std::ios_base::failbit);
    CHECK(!tracer->Extract(iss));
  }

  SECTION(
      "Extracting a span from an invalid binary blob returns "
      "an error.") {
    std::string invalid_context = "abc123xyz321qrs42";
    std::istringstream iss{invalid_context, std::ios::binary};
    auto span_context_maybe = tracer->Extract(iss);
    CHECK(!span_context_maybe);
  }

  SECTION("Calling Extract on an empty stream yields a nullptr.") {
    std::string blob;
    std::istringstream iss(blob, std::ios::binary);
    auto span_context_maybe = tracer->Extract(iss);
    CHECK(span_context_maybe);
    CHECK(span_context_maybe->get() == nullptr);
  }

  SECTION("If a carrier supports LookupKey, then ForeachKey won't be called") {
    CHECK(tracer->Inject(span->context(), text_map_carrier));
    CHECK(text_map.size() == 1);
    text_map_carrier.supports_lookup = true;
    auto span_context_maybe = tracer->Extract(text_map_carrier);
    CHECK((span_context_maybe && span_context_maybe->get()));
    CHECK(text_map_carrier.foreach_key_call_count == 0);
  }

  SECTION(
      "When LookupKey is used, a nullptr is returned if there is no "
      "span_context") {
    text_map.clear();
    text_map_carrier.supports_lookup = true;
    auto span_context_maybe = tracer->Extract(text_map_carrier);
    CHECK((span_context_maybe && span_context_maybe->get() == nullptr));
    CHECK(text_map_carrier.foreach_key_call_count == 0);
  }

  SECTION("Verify only valid base64 characters are used.") {
    CHECK(tracer->Inject(span->context(), text_map_carrier));
    CHECK(text_map.size() == 1);
    // Follows the guidelines given in RFC-4648 on what characters are
    // permissible. See
    //    http://www.rfc-editor.org/rfc/rfc4648.txt
    auto iter = text_map.begin();
    CHECK(iter != text_map.end());
    auto value = iter->second;
    auto is_base64_char = [](char c) {
      return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') ||
             ('0' <= c && c <= '9') || c == '+' || c == '/' || c == '=';
    };
    CHECK(std::all_of(value.begin(), value.end(), is_base64_char));
    CHECK(value.size() % 4 == 0);
  }

  SECTION("Inject fails if inject_error_code is non-zero.") {
    MockTracerOptions tracer_options_fail;
    auto error_code = std::make_error_code(std::errc::network_down);
    tracer_options_fail.propagation_options.inject_error_code = error_code;
    tracer = std::shared_ptr<opentracing::Tracer>{
        new MockTracer{std::move(tracer_options_fail)}};

    std::ostringstream oss;
    auto rcode = tracer->Inject(span->context(), oss);
    CHECK(!rcode);
    CHECK(rcode.error() == error_code);
  }

  SECTION("Extract fails if extract_error_code is non-zero.") {
    MockTracerOptions tracer_options_fail;
    auto error_code = std::make_error_code(std::errc::network_down);
    tracer_options_fail.propagation_options.extract_error_code = error_code;
    tracer = std::shared_ptr<opentracing::Tracer>{
        new MockTracer{std::move(tracer_options_fail)}};

    CHECK(tracer->Inject(span->context(), text_map_carrier));
    auto span_context_maybe = tracer->Extract(text_map_carrier);
    CHECK(!span_context_maybe);
    CHECK(span_context_maybe.error() == error_code);
  }
}
