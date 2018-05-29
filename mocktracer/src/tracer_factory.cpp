#include <opentracing/mocktracer/json_recorder.h>
#include <opentracing/mocktracer/tracer.h>
#include <opentracing/mocktracer/tracer_factory.h>
#include <cctype>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>

namespace opentracing {
BEGIN_OPENTRACING_ABI_NAMESPACE
namespace mocktracer {

namespace {
struct InvalidConfigurationError : public std::exception {
 public:
  InvalidConfigurationError(const char* position, std::string&& message)
      : position_{position}, message_{std::move(message)} {}

  const char* what() const noexcept override { return message_.c_str(); }

  const char* position() const { return position_; }

 private:
  const char* position_;
  std::string message_;
};
}  // namespace

static void Consume(const char*& i, const char* last, string_view s) {
  if (static_cast<std::size_t>(std::distance(i, last)) < s.size()) {
    throw InvalidConfigurationError{i,
                                    std::string{"expected "} + std::string{s}};
  }

  for (size_t index = 0; index < s.size(); ++index) {
    if (*i++ != s[index]) {
      throw InvalidConfigurationError{
          i, std::string{"expected "} +
                 std::string{s.data() + index, s.data() + s.size()}};
    }
  }
}

static void ConsumeWhitespace(const char*& i, const char* last) {
  for (; i != last; ++i) {
    if (!std::isspace(*i)) {
      return;
    }
  }
}

static void ConsumeToken(const char*& i, const char* last, string_view token) {
  ConsumeWhitespace(i, last);
  Consume(i, last, token);
}

static std::string ParseFilename(const char*& i, const char* last) {
  ConsumeToken(i, last, "\"");
  std::string result;
  while (i != last) {
    if (*i == '\"') {
      ++i;
      return result;
    }
    if (*i == '\\') {
      throw InvalidConfigurationError{
          i, "escaped characters are not supported in filename"};
    }
    if (std::isprint(*i)) {
      result.push_back(*i);
    } else {
      throw InvalidConfigurationError{i, "invalid character"};
    }
    ++i;
  }

  throw InvalidConfigurationError{i, R"(no matching ")"};
}

static std::string ParseConfiguration(const char* i, const char* last) {
  ConsumeToken(i, last, "{");
  ConsumeToken(i, last, R"("output_file")");
  ConsumeToken(i, last, ":");
  auto filename = ParseFilename(i, last);
  ConsumeToken(i, last, "}");
  ConsumeWhitespace(i, last);
  if (i != last) {
    throw InvalidConfigurationError{i, "expected EOF"};
  }

  return filename;
}

struct MockTracerConfiguration {
  std::string output_file;
};

expected<std::shared_ptr<Tracer>> MockTracerFactory::MakeTracer(
    const char* configuration, std::string& error_message) const noexcept try {
  MockTracerConfiguration tracer_configuration;
  if (configuration == nullptr) {
    error_message = "configuration must not be null";
    return make_unexpected(invalid_configuration_error);
  }
  try {
    tracer_configuration.output_file = ParseConfiguration(
        configuration, configuration + std::strlen(configuration));
  } catch (const InvalidConfigurationError& e) {
    error_message = std::string{"Error parsing configuration at position "} +
                    std::to_string(std::distance(configuration, e.position())) +
                    ": " + e.what();
    return make_unexpected(invalid_configuration_error);
  }

  errno = 0;
  std::unique_ptr<std::ostream> ostream{
      new std::ofstream{tracer_configuration.output_file}};
  if (!ostream->good()) {
    error_message = "failed to open file `";
    error_message += tracer_configuration.output_file + "` (";
    error_message += std::strerror(errno);
    error_message += ")";
    return make_unexpected(invalid_configuration_error);
  }

  MockTracerOptions tracer_options;
  tracer_options.recorder =
      std::unique_ptr<Recorder>{new JsonRecorder{std::move(ostream)}};

  return std::shared_ptr<Tracer>{new MockTracer{std::move(tracer_options)}};
} catch (const std::bad_alloc&) {
  return make_unexpected(std::make_error_code(std::errc::not_enough_memory));
}

}  // namespace mocktracer
END_OPENTRACING_ABI_NAMESPACE
}  // namespace opentracing
