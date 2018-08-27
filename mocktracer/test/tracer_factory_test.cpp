#include <opentracing/mocktracer/tracer_factory.h>
#include <cstdio>
#include <random>
#include <string>

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>
using namespace opentracing;
using namespace mocktracer;

TEST_CASE("tracer_factory") {
  MockTracerFactory tracer_factory;
  std::string error_message;

  SECTION("Creating a tracer from a nullptr yields an error.") {
    const char* configuration = nullptr;
    auto tracer_maybe = tracer_factory.MakeTracer(configuration, error_message);
    REQUIRE(!tracer_maybe);
    REQUIRE(!error_message.empty());
  }

  SECTION("Creating a tracer from an empty string yields an error.") {
    const char* configuration = "";
    auto tracer_maybe = tracer_factory.MakeTracer(configuration, error_message);
    REQUIRE(!tracer_maybe);
    REQUIRE(!error_message.empty());
  }

  SECTION("Creating a tracer from invalid JSON yields an error.") {
    const char* configuration = "{ abc";
    auto tracer_maybe = tracer_factory.MakeTracer(configuration, error_message);
    REQUIRE(!tracer_maybe);
    REQUIRE(!error_message.empty());
  }

  SECTION(
      "Creating a tracer from valid JSON but an invalid configuration "
      "yields an error.") {
    const char* configuration = R"({ "abc": 123 })";
    auto tracer_maybe = tracer_factory.MakeTracer(configuration, error_message);
    REQUIRE(!tracer_maybe);
    REQUIRE(!error_message.empty());
    REQUIRE(tracer_maybe.error() == invalid_configuration_error);
  }

  SECTION("Creating a tracer with an invalid output_file yields an error.") {
    const char* configuration = R"({ "output_file": "" })";
    auto tracer_maybe = tracer_factory.MakeTracer(configuration, error_message);
    REQUIRE(!tracer_maybe);
    REQUIRE(!error_message.empty());
    REQUIRE(tracer_maybe.error() == invalid_configuration_error);
  }

  SECTION("Creating a tracer with a valid config succeeds.") {
    std::string span_filename{"spans."};
    const auto random_id = std::random_device{}();
    span_filename.append(std::to_string(random_id));
    std::string configuration = R"({ "output_file": ")";
    configuration.append(span_filename);
    configuration.append(R"(" })");

    auto tracer_maybe =
        tracer_factory.MakeTracer(configuration.c_str(), error_message);
    REQUIRE(tracer_maybe);

    std::remove(span_filename.c_str());
  }
}
